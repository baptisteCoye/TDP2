README.md
Auteurs : Baptiste Coye - Mathieu Deschamps

Pour compiler le projet (depuis la source) : 
     Créer un répertoire build.
     Depuis ce répertoire, lancer "cmake ..", puis "make"

Pour tester le projet :
     Dans le répertoire /build/tests, deux exécutables sont créés :
     	  - test_sequ : lance un test en séquentiel en affichant les performances. Ne nécessite pas d'arguments.
	  - test_par  : lance un test en parallèle en affichant les performances. Ne nécessite pas d'arguments.
     	Ces deux tests utilisent le meme fichier d'entree, "/tests/data/file_test.txt" et lancent tous deux le programme sur 100 itérations.

     Dans le répertoire /build/driver, un exécutable "driver" est créé. Il nécessite en entrée le nomnbre d'itérations souhaitées ainsi que le fichier contenant la liste des particules.