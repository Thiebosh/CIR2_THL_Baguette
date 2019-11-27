# théorie des langages

## but du jeu : langage de programmation en français
- min à faire : dire si le programme à traiter est correctement écrit dans notre langage (10/20)
- max (?) : compiler, exécuter, donner le résultat (plus d'options => plus de points)


## Nos spécifications 
langage cible : cpp
améliorations : 
- typage faible des variables (auto en cpp ou spécifie selon format de la variable)
- typage fort des fonctions
- indice -1 d'un tableau pour accéder au dernier élément
- tab[] = val pour allocation dynamique
- boucles for imbriquées (Pour x+1 va de 0 à 10 et y+1 va de 0 à 10 {})
- séparation des nombres 1_000 <=> 1000 (bonne lisibilitée)
- interdire le while true ?

conservé :
- accolades
- déclaration fonctions
- ternaires


## ordre d'avancement
- point de lancement <- avancement actuel
- variables
- tableaux //multidimentionnel
- conditions
- boucles
- fonctions
- lecture fichier ou interprétation à la volée
- listes //élément facilement supressible
- dictionnaires //map associatives
- etc (ternaires)


## pile de choses à faire :
- finir reprise tab
- fonction cast pour associations (fonction prend valAccess et varType, dépile valeur, la transforme si besoin et la rempile pour la variable) - si conversion impossible, fonction erreur
- cast dans les opérations, à la fin des if existants - si conversion impossible, fonction erreur
- fonction (appelle map ?) levée d'erreur (enum d'erreurs associées) pour regrouper textes -> exit(1) le programme
- comparaisons et type "binaire" (booléen)

- ajouter for(initialisation / affectation, test, affectation), foreach dans bison
- implémenter fonctions (deque de fonction (nom - adresse instruction)) sans arguments
- ajouter paramètres aux fonctions (ajoute au deque vecteur des types attendus, crée variables dans l'ordre dans fonction)
- rendre paramètres facultatifs ( fonction(,4,) => fonction(entier var1 = 0, reel var2 = 0, texte var3 = "") )

- mémoire programme (instruction list) opti : vector de bool et types pour enregistrer uniquement info utile(conversion binaire). Pour string, taille puis caractères. Si taille max, flag de fin de texte : 1 si fini, 0 pour continuer à lire string