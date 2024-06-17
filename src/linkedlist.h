/* linkedlist.h */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node *link;
struct node {
	double item;
	link next;
};

link make_node(double item);
void free_node(link p);
link search_node(unsigned char key);
void insert_node(link p);
void delete_node(link p);
void traverse_node(void (*visit)(link));
void destroy_node(void);
void push_node(link p);
link pop_node(void);
double mean_node(void);

#endif
