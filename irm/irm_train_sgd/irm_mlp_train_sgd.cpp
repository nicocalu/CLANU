#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <thread>  // IF4
#include <mutex>   // IF4
#include <csignal> // IF4
#include <random>

#include "MLP_Network_SGD.h"
#include "MLP_Layer_SGD.h"
#include "IRM2D.h"
#include "timing_functions.h"
#include "progress_bar.h"
#include "gnuplot_utilities.h"

using namespace std;

// COMMENT or UNCOMMENT depending of the computer OS, and adjust
// LINUX / MAC
    const std::string SRC_PATH = "C:/Users/nicoc/Downloads/cpp";
    const std::string GNUPLOT_PATH= "C:/msys64/mingw64/bin/gnuplot.exe";

// WINDOWS
//    const std::string SRC_PATH = "D:/Documents/_NextCloud/documents/Insa-GE/Clanu/cpp";
//    const std::string GNUPLOT_PATH="C:\\\"Program Files\"\\gnuplot\\bin\\wgnuplot.exe";

// global variable for exchange information with handler
std::string *optim_log_filename_ptr;
int * maxEpoch_ptr;

void signal_callback_handler(int signum);

std::pair<float, float> ACCURACYandLOSS(MLP_Network_SGD* mlp, int nSet, float** input, char** desiredOutput)
{
    int sums = 0;
    float loss = 0.F;
    for (int i = 0; i < nSet; i++)
    {
        mlp->ForwardPropagateNetwork(input[i]);
        sums += mlp->CalculateResult(desiredOutput[i]);
        loss += mlp->LossFunction(desiredOutput[i]);
    }
    float accuracy = (sums / (float)nSet) * 100;
    loss /= nSet;
    return std::make_pair(accuracy, loss);
}

int main(int argc, char *argv[])
{
    cout << "SGD Train " << endl;
    if( argc < 2)
        {
        cerr << " Usage : " << argv[0] << " file.bin" << endl;
        cerr << " where : file.bin is the file where the network's architecture and weights will be stored" << endl;
        return -1;
        }


    signal (SIGINT, signal_callback_handler);

    int nInputUnit      = 64*64;
    int nOutputUnit     = 9;
    vector<int> nHiddenUnit({16, 16}); // 40;
    int nHiddenLayer    = nHiddenUnit.size();

    float learningRate  = 0.0001;
    int maxEpoch        = 250;
    int nMiniBatch      = 50;

    maxEpoch_ptr = & maxEpoch;   // global variable update

    //IRM2D Input Array Allocation and Initialization
    IRM2D irm;


    cout << " -> Reading train directory : " << SRC_PATH+"/IRM2D/train" << endl;
    int nTrainingSet = irm.ReadPath(SRC_PATH+"/data/IRM2D/train");
    nTrainingSet = 5000;  //! Limit to 1000 images

    float errMinimum    = 0.001;

    //Allocate
    float **inputTraining		    = new float*[nTrainingSet];
    char **desiredOutputTraining	= new char*[nTrainingSet];

    for( int i = 0; i < nTrainingSet; i++ )
    {
        inputTraining[i]		    = new float[nInputUnit]{};
        desiredOutputTraining[i]	= new char[nOutputUnit]{};
    }
    irm.ReadInput(0, nTrainingSet, inputTraining);
    irm.ReadLabel(0, nTrainingSet, desiredOutputTraining);


    irm.SaveImage("a.png", inputTraining[0], 64,64 );

///////////////////////////////////////////////////////////////////////////////////
    cout << " \n \n" ;
    cout << " -> Reading test directory : " << SRC_PATH+"/IRM2D/test" << endl;
    int nTestSet = irm.ReadPath(SRC_PATH+"/data/IRM2D/test");
    nTestSet = 1000;

    float **inputTest			    = new float*[nTestSet];
    char **desiredOutputTest        = new char*[nTestSet];

    for(int i = 0;i < nTestSet;i++)
    {
        inputTest[i]			    = new float[nInputUnit];
        desiredOutputTest[i]    	= new char[nOutputUnit]{};
    }

    int sums=0;
    float accuracyRate=0.F;

    irm.ReadInput(0, nTestSet, inputTest);
    irm.ReadLabel(0, nTestSet, desiredOutputTest);


    MLP_Network_SGD mlp('L');
    mlp.Allocate(nInputUnit,nHiddenUnit,nOutputUnit,nTrainingSet);

    string fullname = SRC_PATH+"/models/"+argv[1];
    cout << "Will write models and weights to : \n \t" <<fullname << endl;
    size_t lastindex = fullname.find_last_of(".");
    string rawname = fullname.substr(0, lastindex);
    cout << "  Rawname for intermediate saving : \n \t" << rawname << endl;
    string best_filename;

// LOG and GRAPH
    string log_filename = rawname + ".dat";
    optim_log_filename_ptr = &log_filename;  // update global variable
    cout << "  Filename for log : \n \t" << log_filename << endl;
    fstream log_stream;
    log_stream.open(log_filename.c_str(),fstream::out | fstream::trunc);// ouverture en écriture
    log_stream << "epoch \t train_{loss} \t test_{loss} \t lr \t train_{acc} \t  test_{acc} \t duration(s)" << endl;

// THREAD FOR REAL TIME UPDATE
    string gnuplot_graph_acc = SRC_PATH+"/models/graph_acc_sgd.plt";
    GenerateGraphAcc(log_filename, gnuplot_graph_acc, maxEpoch, true);
    string system_call_string(GNUPLOT_PATH+" "+gnuplot_graph_acc);
    std::thread gnuplot_thread_acc( system, system_call_string.c_str() );

    string gnuplot_graph_loss = SRC_PATH+"/models/graph_loss_sgd.plt";
    GenerateGraphLoss(log_filename, gnuplot_graph_loss, maxEpoch, true);
    system_call_string = GNUPLOT_PATH+" "+gnuplot_graph_loss;
    std::thread gnuplot_thread_loss( system, system_call_string.c_str() );

// Permutation vector
    std::vector<unsigned int> indexes(nTrainingSet);
    for(unsigned int i=0; i<indexes.size(); i++) indexes[i]=i;


    //Start clock
    clock_t start, finish;
    double elapsed_time;
    start = clock();
    float local_time = 0;

    float initialLR = learningRate;

    float maxTestAccuracyRate = 0;
    int epoch = 0;
    while (epoch < maxEpoch)
        {
        tic();
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indexes.begin(), indexes.end(), g);
        ProgressBar('R');

        float sumError=0;
        int batchCount=0;
        for (int i = 0; i < nTrainingSet; i++)
            {

            if( (i%100) == 0 ) local_time = duration_from_tic();
            if( (i%10) == 0  )
                {
                ProgressBar('P', 1.0*i / nTrainingSet, string(" - " + std::to_string(local_time)).c_str(), 60);
                }

            mlp.ForwardPropagateNetwork(  inputTraining[indexes[i]]         );
            sumError += mlp.LossFunction( desiredOutputTraining[indexes[i]] );
			mlp.BackwardPropagateNetwork( desiredOutputTraining[indexes[i]] );
            

            if( ((batchCount+1) % nMiniBatch) == 0)
                {
                mlp.UpdateWeight(learningRate);
                batchCount=0;
                }
            batchCount++;
            }
        ProgressBar('C');

        sumError /= nTrainingSet;

//TEST ACCURACY and LOSS
        std::pair<float, float> test_metrics = ACCURACYandLOSS(&mlp, nTestSet, inputTest, desiredOutputTest);
        float AccuracyTest = test_metrics.first;
        float LossTest = test_metrics.second;


//TRAIN ACCURACY and LOSS
        std::pair<float, float> train_metrics = ACCURACYandLOSS(&mlp, nTrainingSet, inputTraining, desiredOutputTraining);
        float AccuracyTrain = train_metrics.first;
        float LossTrain = train_metrics.second;

// START CHRONO
        tac();

// DISPLAY AND SAVE LOG
        std::cout << std::fixed << std::setprecision(3);
        if(epoch%30 == 0)
            cout << " epoch | Loss_tn | Loss_tt |     lr    |  ACC_tn  |  ACC_tt  |  duration(s)" << endl;

        cout << std::setw(6) << epoch << " | " << std::setw(7) << LossTrain << " | "<< std::setw(7) << LossTest
             << std::scientific << " | " << std::setw(8) << learningRate
             << std::fixed << " | " << std::setw(8) << AccuracyTrain << " | " << std::setw(8) << AccuracyTest
             << " | " << std::setw(8) << duration();
        log_stream << epoch<< "\t" <<LossTrain<< "\t" << LossTest << "\t" << learningRate
                 << "\t" << AccuracyTrain << "\t" << AccuracyTest << "\t" << duration() << endl;

// SAVE IF CURRENT IS THE BEST
        if(AccuracyTest > maxTestAccuracyRate)
            {
            maxTestAccuracyRate = AccuracyTest;
            best_filename = rawname + "_" + to_string(epoch) + "_" + to_string(AccuracyTest) + ".bin";
            std::ofstream os(best_filename, std::ofstream::binary);
            os<< mlp;
            os.close();
            cout << " * " << endl;
            }
        else cout << endl;

// EARLY STOP IF NO SIGNIFICANT CHANGE
        if (sumError < errMinimum)
            break;

// UPDATES FOR NEXT EPOCH
        learningRate = initialLR/(1+epoch*learningRate);    // learning rate progressive decay
        ++epoch;
    }

    //Finish clock
    finish = clock();
    elapsed_time = (double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"time: "<<elapsed_time<<" sec"<<endl;
    cout << " Best model : " << best_filename << endl;

    log_stream.close();

    //saving best model
    MLP_Network_SGD mlp_best;
    std::ifstream is (best_filename, std::ifstream::binary);
    is >> mlp_best;
    is.close();

    std::ofstream os_f(SRC_PATH+"/models/"+argv[1], std::ofstream::binary);
    os_f << mlp_best;
    os_f.close();


    // Test Set Result
    cout<<"[Result]"<<endl<<endl;

    std::pair<float, float> final_train_metrics = ACCURACYandLOSS(&mlp_best, nTrainingSet, inputTraining, desiredOutputTraining);
    accuracyRate = final_train_metrics.first;

    cout << "[Training Set]\t"<<"Accuracy Rate: " << accuracyRate << " %"<<endl;

    // Test Set Result
    std::pair<float, float> final_test_metrics = ACCURACYandLOSS(&mlp_best, nTestSet, inputTest, desiredOutputTest);
    accuracyRate = final_test_metrics.first;

    cout << "[Test Set]\t"<<"Accuracy Rate: " << accuracyRate << " %"<<endl;


    // Test Set Result
    sums=0;
    accuracyRate=0.F;
    for( int i=0; i<nTestSet; i++)
        {
        mlp_best.ForwardPropagateNetwork(inputTest[i]);
        sums += mlp_best.CalculateResult(desiredOutputTest[i]);
        }
    accuracyRate = (sums / (float)nTestSet) * 100;

    cout << "[Test Set]\t"<<"Accuracy Rate: " << accuracyRate << " %"<<endl;


 // FREE ALLOCATED SPACES
    for( int i=0; i<nTrainingSet; i++)
        {
        delete [] desiredOutputTraining[i];
        delete [] inputTraining[i];
        }

    for( int i=0; i<nTestSet; i++)
        {
        delete [] desiredOutputTest[i];
        delete [] inputTest[i];
        }

    delete[] inputTraining;
    delete[] desiredOutputTraining;
    delete[] inputTest;
    delete[] desiredOutputTest;

    signal_callback_handler(0);

    return 0;
}


// cette fonction est un garde fou pour limiter les conséquences de ceux qui ne lisent pas l'énoncé...
void signal_callback_handler(int signum)
{
   cout << "Caught signal " << signum << endl;

   system("killall gnuplot"); // will work only on linux -> GREAT CODE

   string gnuplot_graph_acc = SRC_PATH+"/models/graph_acc_sgd.plt";
   GenerateGraphAcc(*optim_log_filename_ptr, gnuplot_graph_acc, *maxEpoch_ptr, false);
   string system_call_string(GNUPLOT_PATH+" "+gnuplot_graph_acc);
   std::thread gnuplot_thread_acc( system, system_call_string.c_str() );

   string gnuplot_graph_loss = SRC_PATH+"/models/graph_loss_sgd.plt";
   GenerateGraphLoss(*optim_log_filename_ptr, gnuplot_graph_loss, *maxEpoch_ptr, false);
   system_call_string = GNUPLOT_PATH+" "+gnuplot_graph_loss;
   std::thread gnuplot_thread_loss( system, system_call_string.c_str() );

   exit(signum);
}

