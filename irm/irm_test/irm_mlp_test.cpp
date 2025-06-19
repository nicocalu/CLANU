#include <iostream>
#include <string>

#include "MLP_Network_SGD.h"
#include "MLP_Network_ADAM.h"

#include "IRM2D.h"

#include "timing_functions.h"   //tic() & tac() &  duration() functions

using namespace std;


// Question 2.2.1
// LINUX / MAC
  //  const string SRC_PATH = "/home/grenier/Downloads/Clanu/cpp";
// WINDOWS
    const string SRC_PATH = "C:/Users/nicoc/Downloads/cpp";


int main(int argc, char *argv[])
{
#if defined(_OPENMP)
    cout << " OPENMP is activated : great! " << endl;
#else
    cout << " OPENMP is not activated (good for debug)" << endl;
#endif

#ifdef __FAST_MATH__
    cout << " fast-math is activated : great! " << endl;
#else
    cout << " fast-math is strangely not activated " << endl;
#endif

    if( argc < 2)
        {
        cerr << " Usage : " << argv[0] << " file.bin" << endl;
        cerr << " where : file.bin is the architecture and weights of the network to be loaded" << endl;
        return -1;
        }

// Load Model
    cout << "Reading network models (architecture and weights) from : " << SRC_PATH << "/models/" << argv[1] << endl;
    tic();
    MLP_Network_ADAM mlp;
    std::ifstream is (SRC_PATH+"/models/"+argv[1], std::ifstream::binary);
    is >> mlp;
    is.close();
    tac();
    cout << "Model parameters :" << endl;
    cout << "   Input Neurons:  " << mlp.GetnInputUnit() << endl;
    cout << "   Hidden Neurons: [";
    for (const auto& h : mlp.GetnHiddenUnit()) {
        cout << h << ", ";
    }
    cout << "]" << endl;
    cout << "   Output Neurons: " << mlp.GetnOutputUnit() << endl;
    cout << "   N. of Layers:   " << mlp.GetnHiddenLayer() << endl;
    cout << " Model loaded in : "<< duration() << " s" << endl;


// Reading Images
    int nInputUnit      = 64*64;
    int nOutputUnit     = 9;


    //Input Array Allocation and Initialization
    IRM2D irm2d;

    cout << " Reading directory : " << SRC_PATH+"/data/IRM2D" << "  ";

    int nTestSet = irm2d.ReadPath(SRC_PATH+"/data/IRM2D/test");
    //nTestSet = 1000;
    cout <<  nTestSet << endl;

    //Allocate
    float **inputTest			= new float*[nTestSet];
    char **desiredOutputTest	= new char*[nTestSet];

    for(int i = 0;i < nTestSet;i++)
        {
        inputTest[i]			= new float[nInputUnit]{};
        desiredOutputTest[i]	= new char[nOutputUnit]{};
        }


    // ReadData (images and labels)
    irm2d.ReadInput(0, nTestSet, inputTest);
    irm2d.ReadLabel(0, nTestSet, desiredOutputTest);



    tic();
    //TEST ACCURACY and LOSS
    int sums=0;
    float LossTest=0.F;
    float AccuracyTest=0.F;
    for( int i=0; i<nTestSet; i++)
        {
        mlp.ForwardPropagateNetwork(inputTest[i]);
        sums += mlp.CalculateResult(desiredOutputTest[i]);
        LossTest += mlp.LossFunction( desiredOutputTest[i] );
        }
    AccuracyTest = (sums / (float)nTestSet) * 100;
    LossTest /= nTestSet;
    tac();
    cout << "[Test Set]\t Loss : "<< LossTest << "Accuracy : " << AccuracyTest << " %"<< "   (compute time : "<< duration() << ")" << endl;



//ADD FROM HERE THE CODE THAT PRINT ALL ERRONEOUS PREDICTED IMAGE INDEXES (Question 2.3.2)
    cout << "Indices des images mal classees :" << endl;
    string error_path = SRC_PATH;
    for (int i = 0; i < nTestSet; i++)
    {
        mlp.ForwardPropagateNetwork(inputTest[i]);
        if (mlp.CalculateResult(desiredOutputTest[i]) == 0)
        {
            cout << i << " ";
            //string filename = error_path + "/" + irm2d.Dataset[i];
            //irm2d.SaveImage(filename, inputTest[i], 64, 64);
        }
    }
    cout << endl;
//STOP HERE THE CODE THAT PRINT ALL ERRONEOUS PREDICTED IMAGE INDEXES (no more changes after this line)

// Ask for an image and print it until -1
    int ind_im;
    cout << " which image ? ";
    cin >> ind_im;

while(ind_im != -1)
    {
    IRM2D::PrintImage(inputTest[ind_im], 64, 64);
    mlp.ForwardPropagateNetwork(inputTest[ind_im]);
    int Predicted = mlp.GetLayerNetwork()[mlp.GetnHiddenLayer()].GetMaxOutputIndex();    // récuperation de la valeur prédite par le réseau
    cout << " predicted : " << IRM2D::convert_label(Predicted) << "  -  "; //affichage prédiction et nom du fichier

    // print filename
    cout << irm2d.Dataset[ind_im] << endl;

    cout << " which image ? (-1 to exit) ";
    cin >> ind_im;
    }

    for(int i=0; i<nTestSet; i++)
        {
        delete [] desiredOutputTest[i];
        delete [] inputTest[i];
        }

    delete[] inputTest;
    delete[] desiredOutputTest;
    
    return 0;
}
