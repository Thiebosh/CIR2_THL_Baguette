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
- implémenter fonctions (deque de fonction (nom - adresse instruction)) sans arguments
- ajouter paramètres aux fonctions (ajoute au deque vecteur des types attendus, crée variables dans l'ordre dans fonction)
- rendre paramètres facultatifs ( fonction(,4,) => fonction(entier var1 = 0, reel var2 = 0, texte var3 = "") )

- operation : test logique ou valeur | valeur : comprend les sommes -> but : généraliser les parenthèses pour resoudre conflit (?)
- regrouper toutes les initialisations? (boucle for)
- regrouper toutes les affectations (éléments à gauche du egal) et les valeurs (operation, test... droite du egal) ? (boucle for)

- mémoire programme (instruction list) opti : vector de bool et types pour enregistrer uniquement info utile(conversion binaire). Pour string, taille puis caractères. Si taille max, flag de fin de texte : 1 si fini, 0 pour continuer à lire string