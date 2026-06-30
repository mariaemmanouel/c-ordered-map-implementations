///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT OrderedMap μέσω ADT List (ταξινομημένη)
// + ADT Map (hash table) για γρήγορη αναζήτηση.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>
#include "ADTOrderedMap.h"
#include "ADTList.h"
#include "ADTMap.h"

struct omap_node {
	Pointer key;
	Pointer value;
};

struct omap {
	List list;
	Map map;
	CompareFunc compare;
	DestroyFunc destroy_key, destroy_value;
};

//Δημιουργεί και επιστρέφει ένα OrderedMap
OMap omap_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	assert(compare != NULL);

	OMap omap = malloc(sizeof(*omap));
	omap->list = list_create(free);
	omap->map = map_create(compare, NULL, NULL);
	omap->compare = compare;
	omap->destroy_key = destroy_key;
	omap->destroy_value = destroy_value;
	return omap;
}

//Επιστρέφει τον αριθμό των στοιχείων στο OrderedMap
int omap_size(OMap omap) {
	return list_size(omap->list);
}

//Αναζητά την τιμή που αντιστοιχεί σε ένα κλειδί
Pointer omap_find(OMap omap, Pointer key) {
	OMapNode node = omap_find_node(omap, key);
	//Αν βρεθεί, επιστρέφουμε την τιμή του
	if (node != OMAP_EOF)
	    return omap_node_value(omap, node);

	return NULL;
}

//Εισάγει ένα ζεύγος key-value στο OrderedMap
void omap_insert(OMap omap, Pointer key, Pointer value) {
	ListNode node = map_find(omap->map, key);
	
	//Αν το κλειδί ήδη υπάρχει
	if (node != NULL){
		struct omap_node* data = list_node_value(omap->list, node);
		//Αποδεσμεύουμε την παλιά τιμή
		if (omap->destroy_value != NULL)
			omap->destroy_value(data->value);
		//Αποδεσμεύουμε το παλιό κλειδί
		if (data->key != key){
			map_remove(omap->map, data->key);
			if (omap->destroy_key != NULL)
			    omap->destroy_key(data->key);
			map_insert(omap->map, key, node);
		}
			
		//Αποθηκεύουμε τα νέα δεδομένα
		data->key = key;
		data->value = value;
		return;
	}

	//Διάσχιση όλης της λίστας για να βρούμε τη σωστή θέση εισαγωγής
	for (ListNode node1 = list_first(omap->list); node1 != LIST_EOF; node1 = list_next(omap->list, node1)){
		struct omap_node* data = list_node_value(omap->list, node1);
		int x = omap->compare(key, data->key);
		if (x < 0){
			struct omap_node* new = malloc(sizeof(*new));
			new->key = key;
			new->value = value;
			//Το εισάγουμε ακριβώς πριν τον τρέχων κόμβο και στο Map
			ListNode prev = list_previous(omap->list, node1);
			list_insert_next(omap->list, prev, new);
			ListNode node2 = (prev == LIST_BOF) ? list_first(omap->list) : list_next(omap->list, prev);
			map_insert(omap->map, key, node2);
			return;
		}
	}

	//Αν το loop τελείωσε χωρίς να βρεθεί μεγαλύτερο κλειδί
	struct omap_node* new = malloc(sizeof(*new));
	new->key = key;
	new->value = value;
	//Το εισάγουμε στο τέλος της λίστας και στο Map
	list_insert_next(omap->list, list_last(omap->list), new);
	map_insert(omap->map, key, list_last(omap->list));
}

//Αφαιρεί ένα στοιχείο με βάση το κλειδί του
bool omap_remove(OMap omap, Pointer key) {
	//Εντοπίζουμε τον κόμβο προς διαγραφή
	ListNode node = map_find(omap->map, key);

	//Αν δεν υπάρχει, επιστρέφουμε false
	if (node == NULL)
	    return false;

    struct omap_node* data = list_node_value(omap->list, (ListNode)node);
	//Απελευθερώνουμε τα εσωτερικά δεδομένα του κόμβου
	if (omap->destroy_value != NULL)
		omap->destroy_value(data->value);
	if (omap->destroy_key != NULL)
		omap->destroy_key(data->key);
	//Διαγράφουμε τον κόμβο και επιστρέφουμε true
	list_remove_next(omap->list, list_previous(omap->list, node));
	map_remove(omap->map, key);
	return true;
}

//Ορίζει τη destroy key function
DestroyFunc omap_set_destroy_key(OMap omap, DestroyFunc destroy_key) {
	DestroyFunc old = omap->destroy_key;
	omap->destroy_key = destroy_key;
	return old;
}

//Ορίζει τη destroy value function
DestroyFunc omap_set_destroy_value(OMap omap, DestroyFunc destroy_value) {
	DestroyFunc old = omap->destroy_value;
	omap->destroy_value = destroy_value;
	return old;
}

//Απελευθερώνει όλη τη μνήμη που δεσμεύει το OrderedMap
void omap_destroy(OMap omap) {
	//Αδειάζουμε ένα-ένα στοιχείο της λίστας
	while (list_size(omap->list) > 0){
		ListNode first = list_first(omap->list);
		struct omap_node* data = list_node_value(omap->list, first);
		omap_remove(omap, data->key);
	}
	//Καταστρέφουμε τη λίστα, το map και το omap
	list_destroy(omap->list);
	map_destroy(omap->map);
	free(omap);
}

//Επιστρέφει τον πρώτο κόμβο
OMapNode omap_first(OMap omap) {
	ListNode node = list_first(omap->list);
	return node == LIST_EOF ? OMAP_EOF : (OMapNode)node;
}

//Επιστρέφει τον τελευταίο κόμβο
OMapNode omap_last(OMap omap) {
	ListNode node = list_last(omap->list);
	return node == LIST_BOF ? OMAP_BOF : (OMapNode)node;
}

//Μετάβαση στον επόμενο κόμβο
OMapNode omap_next(OMap omap, OMapNode node) {
	if (node == OMAP_EOF)
	    return OMAP_EOF;
	ListNode next = list_next(omap->list, (ListNode)node);
	return next == LIST_EOF ? OMAP_EOF : (OMapNode)next;
}

//Μετάβαση στον προηγούμενο κόμβο
OMapNode omap_previous(OMap omap, OMapNode node) {
	if (node == OMAP_BOF)
	    return OMAP_BOF;
	ListNode previous = list_previous(omap->list, (ListNode)node);
	return previous == LIST_BOF ? OMAP_BOF : (OMapNode)previous;
}

//Εντοπίζει και επιστρέφει έναν συγκεκριμένο κόμβο με βάση το κλειδί του
OMapNode omap_find_node(OMap omap, Pointer key) {
	return (OMapNode)map_find(omap->map, key);
}

//Επιστρέφει το κλειδί ενός κόμβου
Pointer omap_node_key(OMap omap, OMapNode node) {
	if (node == OMAP_EOF || node == OMAP_BOF)
	    return NULL;
	
	//Εξάγουμε το key πεδίο από το εσωτερικό struct
	struct omap_node* node1 = list_node_value(omap->list, (ListNode)node);
	return node1->key;
}

//Επιστρέφει την τιμή ενός κόμβου
Pointer omap_node_value(OMap omap, OMapNode node) {
	if (node == OMAP_EOF || node == OMAP_BOF)
	    return NULL;
	
	//Εξάγουμε το value πεδίο από το εσωτερικό struct
	struct omap_node* node1 = list_node_value(omap->list, (ListNode)node);
	return node1->value;
}

//Προωθεί τη hash function στο Map
void omap_set_hash_function(OMap omap, HashFunc hash_func) {
	map_set_hash_function(omap->map, hash_func);
}
