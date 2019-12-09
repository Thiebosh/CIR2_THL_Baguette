# théorie des langages

## pile de choses à faire :
- ajouter boucle do while au menu principal
- essayer de passer les variables globales en local?
- finir reprise tab
- ajouter foreach dans bison
- ajouter operations modulo et puissance
- indice -1 d'un tableau pour accéder au dernier élément
- ajouter length (taille) : retourne taille string ou nb digits nombre
- ajouter IO sleep
- autoriser retours vide ou tableau
- rendre paramètres facultatifs ( fonction(,4,) => fonction(entier var1 = 0, reel var2 = 0, texte var3 = "") )

- generaliser variables aux tableaux : si nom introuvable dans les variables, chercher dans les tableaux. si trouvé, depiler case et continuer pareil


## aller plus loin : 
- mémoire programme (instruction list) opti : vector de bool et types pour enregistrer uniquement info utile(conversion binaire). Pour string, taille puis caractères. Si taille max, flag de fin de texte : 1 si fini, 0 pour continuer à lire string
    - definir les capacités des types (define) et ecrire ces fonctions dans processing.cpp
    - fonction conversion en booleen (passe tout en paramètres, retourne un vector de bool)
        - nombre : prend autant de bits que le vrai type
        - texte : lettre a lettre (sur autant de bits que vrai type) - donne long tableau mais osef
    - fonction conversion en valeur d'origine (passe tout en parametre en adresse, retourne le type ou met sur la pile?)