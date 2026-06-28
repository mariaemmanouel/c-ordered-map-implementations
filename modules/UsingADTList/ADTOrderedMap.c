///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT OrderedMap μέσω ADT List (ταξινομημένη).
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>
#include "ADTOrderedMap.h"
#include "ADTList.h"

struct omap_node {
	Pointer key;
	Pointer value;
};

struct omap {
	List list;
	CompareFunc compare;
	DestroyFunc destroy_key, destroy_value;
};

//Δημιουργεί και επιστρέφει ένα OrderedMap
OMap omap_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	assert(compare != NULL);

	OMap omap = malloc(sizeof(*omap));
	omap->list = list_create(free);
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
	//Διάσχιση όλης της λίστας για να βρούμε τη σωστή θέση εισαγωγής
	for (ListNode node = list_first(omap->list); node != LIST_EOF; node = list_next(omap->list, node)){
		struct omap_node* data = list_node_value(omap->list, node);
		int x = omap->compare(key, data->key);
		
		//Αν το κλειδί ήδη υπάρχει
		if (x == 0){
			//Αποδεσμεύουμε την παλιά τιμή
			if (omap->destroy_value != NULL)
			    omap->destroy_value(data->value);
			//Αποδεσμεύουμε το παλιό κλειδί
			if (omap->destroy_key != NULL && data->key != key)
			    omap->destroy_key(data->key);
			//Αποθηκεύουμε τα νέα δεδομένα
			data->key = key;
			data->value = value;
			return;
		}
		//Αν βρήκαμε μεγαλύτερο κλειδί
		else if (x < 0){
			struct omap_node* new = malloc(sizeof(*new));
			new->key = key;
			new->value = value;
			//Το εισάγουμε ακριβώς πριν τον τρέχων κόμβο 
			list_insert_next(omap->list, list_previous(omap->list, node), new);
			return;
		}
	}

	//Αν το loop τελείωσε χωρίς να βρεθεί ίσο ή μεγαλύτερο κλειδί
	struct omap_node* new = malloc(sizeof(*new));
	new->key = key;
	new->value = value;
	//Το εισάγουμε στο τέλος της λίστας
	list_insert_next(omap->list, list_last(omap->list), new);
}

//Αφαιρεί ένα στοιχείο με βάση το κλειδί του
bool omap_remove(OMap omap, Pointer key) {
	//Εντοπίζουμε τον κόμβο προς διαγραφή
	OMapNode node = omap_find_node(omap, key);
	//Αν δεν υπάρχει, επιστρέφουμε false
	if (node == OMAP_EOF)
	    return false;

	struct omap_node* data = list_node_value(omap->list, (ListNode)node);
	//Απελευθερώνουμε τα εσωτερικά δεδομένα του κόμβου
	if (omap->destroy_value != NULL)
		omap->destroy_value(data->value);
	if (omap->destroy_key != NULL)
		omap->destroy_key(data->key);
	//Διαγράφουμε τον κόμβο και επιστρέφουμε true
	list_remove_next(omap->list, list_previous(omap->list, (ListNode)node));
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
	//Καταστρέφουμε τη λίστα και το omap
	list_destroy(omap->list);
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
	//Διάσχιση όλης της λίστας για να βρούμε το κλειδί
	for (ListNode node = list_first(omap->list); node != LIST_EOF; node = list_next(omap->list, node)){
		struct omap_node* node1 = list_node_value(omap->list, node);
		if (omap->compare(node1->key, key) == 0)
		    return (OMapNode)node;
	}

	return OMAP_EOF;
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

void omap_set_hash_function(OMap omap, HashFunc hash_func) {
}
