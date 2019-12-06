# théorie des langages

## but du jeu : langage de programmation en français
- min à faire : dire si le programme à traiter est correctement écrit dans notre langage (10/20)
- max (?) : compiler, exécuter, donner le résultat (plus d'options => plus de points)


## Nos spécifications 
langage cible : cpp
améliorations : 
- indice -1 d'un tableau pour accéder au dernier élément
- boucles for imbriquées (Pour x+1 va de 0 à 10 et y+1 va de 0 à 10 {})
- séparation des nombres 1_000 <=> 1000 (bonne lisibilitée)


## pile de choses à faire :
- finir reprise tab
- ajouter foreach dans bison
- ajouter operations modulo et puissance
- ajouter IO sleep
- implémenter fonctions (deque de fonction (nom - adresse instruction))
- autoriser retours vide ou tableau
- rendre paramètres facultatifs ( fonction(,4,) => fonction(entier var1 = 0, reel var2 = 0, texte var3 = "") )

- generaliser variables aux tableaux : si nom introuvable dans les variables, chercher dans les tableaux. si trouvé, depiler case et continuer pareil

- mémoire programme (instruction list) opti : vector de bool et types pour enregistrer uniquement info utile(conversion binaire). Pour string, taille puis caractères. Si taille max, flag de fin de texte : 1 si fini, 0 pour continuer à lire string
    - definir les capacités des types (define) et ecrire ces fonctions dans processing.cpp
    - fonction conversion en booleen (passe tout en paramètres, retourne un vector de bool)
        - nombre : prend autant de bits que le vrai type
        - texte : lettre a lettre (sur autant de bits que vrai type) - donne long tableau mais osef
    - fonction conversion en valeur d'origine (passe tout en parametre en adresse, retourne le type ou met sur la pile?)