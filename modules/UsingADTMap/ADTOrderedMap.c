///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT OrderedMap μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>
#include "ADTOrderedMap.h"
#include "ADTMap.h"

// ΣΗΜΕΙΩΣΗ
// Το struct omap_node δεν χρειάζεται σε αυτή την υλοποίηση, θα κάνουμε απλά cast ένα MapNode σε OMapNode

struct omap {
	Map map;
	CompareFunc compare;
};

//Δημιουργεί και επιστρέφει ένα OrderedMap
OMap omap_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	assert(compare != NULL);

	OMap omap = malloc(sizeof(*omap));
	omap->map = map_create(compare, destroy_key, destroy_value);
	omap->compare = compare;
	return omap;
}

//Επιστρέφει τον αριθμό των στοιχείων στο OrderedMap
int omap_size(OMap omap) {
	return map_size(omap->map);
}

//Αναζητά την τιμή που αντιστοιχεί σε ένα κλειδί
Pointer omap_find(OMap omap, Pointer key) {
	return map_find(omap->map, key);
}

//Εισάγει ένα ζεύγος key-value στο OrderedMap
void omap_insert(OMap omap, Pointer key, Pointer value) {
	map_insert(omap->map, key, value);
}

//Αφαιρεί ένα στοιχείο με βάση το κλειδί του
bool omap_remove(OMap omap, Pointer key) {
	return map_remove(omap->map, key);
}

//Ορίζει τη destroy key function
DestroyFunc omap_set_destroy_key(OMap omap, DestroyFunc destroy_key) {
	return map_set_destroy_key(omap->map, destroy_key);
}

//Ορίζει τη destroy value function
DestroyFunc omap_set_destroy_value(OMap omap, DestroyFunc destroy_value) {
	return map_set_destroy_value(omap->map, destroy_value);
}

//Απελευθερώνει όλη τη μνήμη που δεσμεύει το OrderedMap
void omap_destroy(OMap omap) {
	map_destroy(omap->map);

	free(omap);
}

//Επιστρέφει τον κόμβο με το ελάχιστο κλειδί
OMapNode omap_first(OMap omap) {
	MapNode first = map_first(omap->map);
	if (first == MAP_EOF || first == NULL)
	    return OMAP_EOF;

	MapNode min = first;
	//Διάσχιση όλου του Map για τον εντοπισμό του μικρότερου κλειδιού
	for (MapNode node = map_next(omap->map, first); node != MAP_EOF; node = map_next(omap->map, node)){
		Pointer node_key = map_node_key(omap->map, node);
		Pointer min_key = map_node_key(omap->map, min);
		//Αν το τρέχον κλειδί είναι μικρότερο από το ελάχιστο
		if (omap->compare(node_key, min_key) < 0)
		    min = node;
	}

	return min == MAP_EOF ? OMAP_EOF : (OMapNode)min;
}

//Επιστρέφει τον κόμβο με το μέγιστο κλειδί
OMapNode omap_last(OMap omap) {
	MapNode first = map_first(omap->map);
	if (first == MAP_EOF || first == NULL)
	    return OMAP_EOF;

	MapNode max = first;
	//Διάσχιση όλου του Map για τον εντοπισμό του μεγαλύτερου κλειδιού
	for (MapNode node = map_next(omap->map, first); node != MAP_EOF; node = map_next(omap->map, node)){
		Pointer node_key = map_node_key(omap->map, node);
		Pointer max_key = map_node_key(omap->map, max);
		//Αν το τρέχον κλειδί είναι μεγαλύτερο από το μέγιστο
		if (omap->compare(node_key, max_key) > 0)
		    max = node;
	}

	return max == MAP_EOF ? OMAP_EOF : (OMapNode)max;
}

//Επιστρέφει τον αμέσως επόμενο κόμβο
OMapNode omap_next(OMap omap, OMapNode node) {
	if (node == OMAP_EOF || node == NULL)
	    return OMAP_EOF;
	
	Pointer node_key = map_node_key(omap->map, (MapNode)node);
	MapNode next = MAP_EOF;

	//Διάσχιση όλου του Map για να βρούμε το αμέσως μεγαλύτερο στοιχείο
	for (MapNode node2 = map_first(omap->map); node2 != MAP_EOF; node2 = map_next(omap->map, node2)){
		Pointer node2_key = map_node_key(omap->map, node2);
		
		//Αν το κλειδί του τρέχων κόμβου είναι μεγαλύτερο από το δοσμένο
		if (omap->compare(node2_key, node_key) > 0){
			//Αν δεν βρήκαμε κανένα υποψήφιο μέχρι τώρα, το αποθηκεύουμε αυτό
			if (next == MAP_EOF)
			    next = node2;
			//Αλλιώς, ελέγχουμε αν ο νέος υποψήφιος είναι μικρότερος από τον προηγούμενο και τον αποθηκεύουμε
			else{
				Pointer next_key = map_node_key(omap->map, next);

				if (omap->compare(node2_key, next_key) < 0)
				    next = node2;
			}
		}
	}

	return next == MAP_EOF ? OMAP_EOF : (OMapNode)next;
}

//Επιστρέφει τον αμέσως προηγούμενο κόμβο
OMapNode omap_previous(OMap omap, OMapNode node) {
	if (node == OMAP_EOF || node == NULL)
	    return OMAP_EOF;
	
	Pointer node_key = map_node_key(omap->map, (MapNode)node);
	MapNode previous = MAP_EOF;

	//Διάσχιση όλου του Map για να βρούμε το αμέσως μικρότερο στοιχείο
	for (MapNode node2 = map_first(omap->map); node2 != MAP_EOF; node2 = map_next(omap->map, node2)){
		Pointer node2_key = map_node_key(omap->map, node2);
		
		//Αν το κλειδί του τρέχων κόμβου είναι μικρότερο από το δοσμένο
		if (omap->compare(node2_key, node_key) < 0){
			//Αν δεν βρήκαμε κανένα υποψήφιο μέχρι τώρα, το αποθηκεύουμε αυτό
			if (previous == MAP_EOF)
			    previous = node2;
			//Αλλιώς, ελέγχουμε αν ο νέος υποψήφιος είναι μεγαλύτερος από τον προηγούμενο και τον αποθηκεύουμε
			else{
				Pointer previous_key = map_node_key(omap->map, previous);

				if (omap->compare(node2_key, previous_key) > 0)
				    previous = node2;
			}
		}
	}

	return previous == MAP_EOF ? OMAP_EOF : (OMapNode)previous;
}

//Εντοπίζει και επιστρέφει έναν συγκεκριμένο κόμβο με βάση το κλειδί του
OMapNode omap_find_node(OMap omap, Pointer key) {
	MapNode node = map_find_node(omap->map, key);
	return node == MAP_EOF ? OMAP_EOF : (OMapNode)node;
}

//Επιστρέφει το κλειδί ενός κόμβου
Pointer omap_node_key(OMap omap, OMapNode node) {
	return map_node_key(omap->map, (MapNode)node);
}

//Επιστρέφει την τιμή ενός κόμβου
Pointer omap_node_value(OMap omap, OMapNode node) {
	return map_node_value(omap->map, (MapNode)node);
}

//Προωθεί τη hash function στο Map
void omap_set_hash_function(OMap omap, HashFunc hash_func) {
	map_set_hash_function(omap->map, hash_func);
}
