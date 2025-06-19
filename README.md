## 2. Questions IF2

Une partie importante du travail consiste à analyser le code fourni et son fonctionnement. Ce code est long et complexe : il prend en compte les éléments de programmation de toute la formation en informatique GE (IF1, IF2, IF3 et IF4).  
Il est écrit en C++ et non en C (`printf → cout`, `scanf → cin`, `malloc → new`, …).  

Un des objectifs de cette formation est de vous faire adopter une méthode d’analyse de code vous permettant de ne pas vous égarer dans les détails.  
Il vous sera demandé quelques développements de fonctions et la consignation de résultats ou d’observations dans un "rapport". Même s’il n’y a pas de rapport à rendre officiellement, il est demandé de rédiger vos réponses et vos éléments de compréhension en vue de l’évaluation.  
Votre rapport et votre code seront les seuls éléments autorisés lors de l’évaluation individuelle.
### 2.1 Projet QtCreator

1. Récupérez le code du projet sur Moodle (dossier **3GE-IF2-Projet Clanu**).  
2. Ouvrez avec QtCreator le projet `MLP_ADAM.pro`.  
3. Ce projet contient deux bibliothèques et le code à modifier.
### 2.2 Rappels des prérequis

Maîtrise des concepts vus en IF1 :  
- Revoir les TD C (projet **impedance** avec gnuplot qui sera utilisé) ;  
- Projet codage/décodage en ligne de commande et modification du `.pro`.  

Conseils :  
1. Installez ou mettez à jour votre version de QtCreator (voir NoteCpp.pdf, chapitre 3 ou ressources en ligne).  
2. Au besoin, revoyez comment compiler et exécuter un projet avec QtCreator (modes **Debug** et **Release**).  
3. Au besoin, repassez sur la gestion des arguments en ligne de commande (NoteCpp.pdf, chapitre 3) et l’usage d’un terminal via QtCreator.  
4. Installez gnuplot (disponible sous Linux, macOS et Windows).

### 2.3 Prise en main du projet

Dans cette partie, la performance d’un réseau de neurones pré-entraîné sur 10 000 images sera mesurée sur le jeu de données de test, à partir de la source `irm_test` disponible dans le projet **MLP_ADAM**.

1. Dans `irm_mlp_test.cpp`, ajustez la variable `SRC_PATH` au début du fichier de manière à pointer vers le répertoire de votre projet.
2. Exécutez le programme `irm_mlp_test` en lui passant, en argument, le chemin du fichier de réseau `models/best/irm2d_adam_20000.bin`.  
   - Dans QtCreator : **Projets → Run → Command Line Arguments**.
3. Dans votre rapport, donnez un exemple d’exécution.  
   - La précision de ce réseau sur 1 000 images de test est-elle satisfaisante par rapport à vos expérimentations précédentes en MA 1 ?  
   - Qu’entend-on par “précision” exactement ? 
   > La précision (en anglais, accuracy) est une métrique d'évaluation qui mesure la proportion de prédictions correctes faites par votre réseau de neurones sur un ensemble de données. En termes simples, elle répond à la question : "Sur toutes les images que nous avons testées, quel pourcentage le modèle a-t-il correctement identifié ?"
   Comment est-elle calculée et en quoi est-elle plus appropriée que la fonction de coût pour évaluer la performance du réseau ?
   > Le calcul se base sur une comparaison directe entre la sortie prédite par le modèle et la véritable étiquette (la classe attendue).
   > La fonction de coût (loss function) et la précision ont des rôles différents et complémentaires :
   > - La fonction de coût (Loss) est principalement utilisée pendant l'entraînement du réseau. L'objectif de l'entraînement est de minimiser cette fonction. 
   > - La précision (Accuracy) est principalement utilisée pour l'évaluation de la performance finale du modèle. 
4. Modifiez le code pour que la précision soit calculée sur la totalité des images de test.  
   > Ca suffit ce commenter la lighe 64 du irm_mlp_test.cpp qui limite le nombre d'images
   - La précision a-t-elle évolué ? Qu’en concluez-vous sur le réseau et sur les images ?
   > La precision a diminue un peu, de 98.9 % a 98.8146 %. On peut donc conclure que le reseau etait en leger surapprentissage, et que le jeu de test complet est plus variee. 
5. Passez le projet en mode **Debug** pour pouvoir poser des breakpoints.  
   - À l’aide du débogage, déterminez l’architecture du réseau pré-entraîné `models/best/irm2d_adam_20000.bin` (nombre de couches et nombre de neurones par couche) ainsi que les fonctions d’activation utilisées.
   > Modifie un peu le code por en sortir:
   ```
   Model parameters :
   Input Neurons:  4096
   Output Neurons: 9
   N. of Layers:   4
   Hidden Layer size: [32, 32, 24, 16]
   ```
### 2.4 Premiers développements : représentation des données et prédictions

On se focalise sur la représentation des données (en C/C++) et la manipulation des fonctions pour effectuer une prédiction (ou inférence).

1. Dans le fichier `libread/IRM2D.cpp`, répondez aux commentaires de la fonction d’affichage  
   ```cpp
   void PrintImage(float *image, int r, int c);
   ```  
   Cette fonction permet d’afficher les valeurs d’une matrice 64×64 dans la console.  
   - Comment une matrice est-elle représentée en mémoire ?  
   > Dans ce code, une matrice de R lignes et C colonnes est représentée en mémoire comme un tableau à une seule dimension de taille R * C.
   - Comment accède-t-on à l’élément (l, m) d’une matrice ?  
   > On utilise la formule suivante : im[l * C + m] (im un pointeur vers le debut du tableau)
   - Dans le rapport, illustrez par un exemple.
2. Dans `irm_mlp_test.cpp`, en vous inspirant de la boucle `while`, ajoutez dans le `main` une boucle qui affiche les indices de toutes les images mal classées.
> Lignes 112 a 121.
3. Dans le rapport, montrez les lignes de code permettant de faire une prédiction avec le réseau de neurones et d’en déduire la classe à partir d’un encodage one-hot et de la sortie du réseau.
> Lignes 129 a 134:
   ```cpp
   while(ind_im != -1) {
      IRM2D::PrintImage(inputTest[ind_im], 64, 64);
      mlp.ForwardPropagateNetwork(inputTest[ind_im]);
      int Predicted = mlp.GetLayerNetwork()[mlp.GetnHiddenLayer()].GetMaxOutputIndex();    // récuperation de la valeur prédite par le réseau
      cout << " predicted : " << IRM2D::convert_label(Predicted) << "  -  "; //affichage prédiction et nom du fichier
   ```
4. Modifiez `irm_mlp_test.cpp` pour enregistrer les images mal classées à l’aide de la fonction `SaveImage` (voir `libread/IRM2D.h` et `libread/IRM2D.cpp`).  
   - Cette fonction sera appelée par `irm2D.SaveImage(...)` dans le `main`.
> Decommentez les lignes 120 et 121 pour activer la fonctionalite

### 2.5 Deuxième développement : entraînement, sauvegarde et ressources

On s’intéresse maintenant à l’apprentissage du réseau par descente de gradient simple. Le fichier principal est `irm_train_sgd.cpp`.

1. Ajustez les variables `SRC_PATH` et `GNUPLOT_PATH` au début du fichier pour pointer vers le répertoire du projet et l’exécutable gnuplot.  
2. Ce programme prend en argument, lors de son exécution, le nom du fichier dans lequel enregistrer les poids du réseau.
3. Exécutez le programme et comparez les exécutions en modes **Debug** et **Release** :  
   - Temps de calcul  
   - Valeurs obtenues  
   Dans votre rapport, décrivez également comment sont réalisés en C/C++ :  
   1. Les affichages dans la console  
   2. Les tracés avec gnuplot  
   3. La création des fichiers de poids
4. Allégez la fonction `main` en créant une fonction `ACCURACYandLOSS` qui renverra les valeurs d’accuracy et de loss pour un jeu de données et un réseau.  
   - Les objets `mlp` doivent impérativement être passés par adresse.  
   - Cette fonction sera appelée quatre fois dans le `main`.
   > Reemplacement aux lignes 203, 209, 270 et 276.
5. Pour un même réseau, comparez le temps moyen d’entraînement pour une époque sur au moins deux machines différentes.  
   - Dans le rapport, présentez un tableau indiquant :  
     - Le(s) compilateur(s) utilisé(s)  
     - Le(s) système(s) d’exploitation  
     - La référence du processeur  
     - La quantité de mémoire RAM  
     - Le temps d’apprentissage
6. Vous pouvez tester différentes architectures du réseau pour améliorer les résultats, mais ne perdez pas trop de temps (ceci n’est pas l’objectif principal).

### 2.6 Troisième développement : optimisation ADAM

Il s’agit d’implémenter l’optimisation ADAM.

1. Ajoutez un exécutable `irm_train_adam` au projet.  
   - Pour cela, dupliquez le dossier `irm_train_sgd` dans un nouveau répertoire `irm_train_adam`.  
   - Renommez ensuite les fichiers `.cpp`, `.h` et les fichiers `.pro` (celui de la racine et, si besoin, `MLP_ADAM.pro`).
2. Dans `irm_train_adam.cpp`, remplacez les en-têtes  
   ```cpp
   #include "MLP_Network_SGD.h"
   #include "MLP_Layer_SGD.h"
   ```  
   par  
   ```cpp
   #include "MLP_Network_ADAM.h"
   #include "MLP_Layer_ADAM.h"
   ```  
   et adaptez les types des variables `mlp` et `mlp_best`.
   > 13 et 14 changement des headers, 123 et 258 changement de type
3. Complétez la méthode `UpdateWeight` dans `MLP_Layer_ADAM.cpp` en vous inspirant de celle de `MLP_Layer_SGD.cpp`.  
   - Utilisez `MW` et `MW_next` pour _m<sub>ij</sub>_ et sa mise à jour, `SW` et `SW_next` pour _s<sub>ij</sub>_, et de même pour les biais.  
   - Ces variables sont déjà allouées et initialisées ; `alpha_T`, `Beta_1` et `Beta_2` sont disponibles.  
   - Désactivez la création des courbes gnuplot (commentez les deux lignes `std::thread gnuplot_thread_…`).  
   - Réduisez le nombre d’images pour l’entraînement et le test (accélère les itérations de développement).  
   - Recompilez régulièrement tout le projet via **Compiler → Tout recompiler**.
4. Pour un même réseau (par exemple) :
   ```cpp
   int    nInputUnit    = 64*64;
   int    nOutputUnit   = 9;
   vector<int> nHiddenUnit({32, 16});
   int    nHiddenLayer  = 2;

   float  learningRate  = 0.0001f;
   int    maxEpoch      = 250;
   int    nMiniBatch    = 50;
   ```
   comparez, pour SGD et ADAM :
   - Temps par époque  
   - Évolution des valeurs de coût et d’efficacité  
   Consignez vos observations et vos conclusions.
5. Ajustez les paramètres du réseau et ceux d’entraînement pour obtenir une bonne précision.  
   - Sauvegardez vos réseaux entraînés sous différents noms.  
   - Dans le rapport, indiquez les paramètres du réseau, les hyperparamètres retenus et les performances obtenues, puis comparez-les à celles de `models/best/irm_ADAM.bin`.
```
