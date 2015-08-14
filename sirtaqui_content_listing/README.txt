
L'entrée "order_by" de $query est un tableau.

La toute première entrée de ce tableau correspond au "sorting" de SCL (mais pas obligatoirement - voir plus-loin : "cas particuliers").

Ensuite viennent les hook_scl_search() qui sont appelés dans le même ordre que celui dans lequel ils sont listés dans le formulaire de recherche.
Chacun de ces hook possède un setting (propre à chaque module), qui permet de déterminer la manière dont va être ajoutée une éventuelle clause d'order à $query['order_by'] :
- none : pas de modif
- prepend : array_unshift()
- append : array_push()
- replace : = array(la clause)

Puis viennent les hook_scl_query() qui sont appelés juste avant la compilation SQL.
Ils sont appelés (ceux qui ne sont pas paramétrés à 0) dans l'ordre établi dans les settings de SCL.
Les hooks ne sont pas du-tout appelés si l'option 'hook_query' du compilateur est à false.

Cas particuliers:
- Si le hook sirtaqui_content_listing_scl_query() est paramétré sur 0 ou l'option 'hook_query' du compilateur est à false, le sorting de SCL sera exécuté avant tous les autres hooks, peuplant ainsi la première entrée du tableau $query['order_by'].	
	Ce sorting n'est malgré-tout pas exécuté si :
		- une $query contenant déjà une clause order_by est fournie au compilateur (cas de la preview du formulaire d'édition du sorting)
		- le cache de sorting de SCL est vide
				
Les hooks_scl_search() de certains modules peuvent parfois déléguer leur traitements au hook_scl_query() de ce même module.
A l'instar des modes d'insertion des clauses d'order, cela est défini dans le form setting de chaque module.
C'est notamment le cas pour prestataire_dispos, pour lequel on peut choisir de déléguer le traitement d'une recherche par dispos au hook prestataires_dispos_scl_query(). Cela permet d'avoir un comportement différent, selon que l'on effectue une recherche, ou un simple affichage du listing.

