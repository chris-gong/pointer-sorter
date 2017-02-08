#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <ctype.h>

  // TODO figure out why the tree doesn't print right for
  // the dictionary with a lot of words

enum color {RED, BLACK}; // RED == 0, BLACK == 1

typedef struct Node_ Node;
typedef struct Tree_ Tree;

struct Tree_{
  Node *root;
};

struct Node_{
  enum color col;
  char *word;
  Node *left, *right, *parent;
};

Tree *initTree(Tree * tree) {
  tree->root = NULL;
  return tree;
}

Node *initNode(Node *node, enum color col, char *word) {
  node->col = col;
  node->word = word;
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  return node;
}


void print(Node *node, char *prefix, int isTail) {
  //char *tmp = malloc(strlen(prefix) + strlen(node->word) + 1);
  //memcpy(tmp, prefix, strlen(prefix));
  //memcpy(tmp + strlen(prefix), "bla bla", strlen("bla bla"));
  // add null byte 
  if (node == NULL) return;
  
  // allocate space of length strlen(prefix) + 4 (see ternary)
  // + 1 for null byte

  char tmp[strlen(prefix) + 4 + 1]; // TODO do this with malloc
//  char *tmp = malloc(strlen(prefix) + 4 + 1);
  sprintf(tmp, "%s%s", prefix, (isTail ? "    " : "│   "));
//  printf("tmp: %s\n", tmp);
  printf("%s%s%s (%s)\n", prefix , (isTail ? "└── " : "├── ") , 
    node->word, (node->col == RED ? "red": "black"));
 
  print(node->left, tmp, 0);

  print(node->right, tmp, 1);

//  free(tmp);
}


void BSTinorder(Node *node) {
  if (node == NULL) return;
 
  // keep going left
  BSTinorder(node->left);  

  // print node
  printf("%s\n", node->word);

  // go right
  BSTinorder(node->right);

}

void printTree(Tree *t) {
    print(t->root, "", 1);
}


void freeNodes(Node *node) {
  if (node == NULL) return;
  freeNodes(node->left);
  
  freeNodes(node->right);

  // free the node and the string:
  free(node->word);
  free(node);
}


// frees the memory allocated for the nodes and the
// words associated with them
void freeNodesInTree(Tree *t) {
  freeNodes(t->root);
}


void BSTinsert(Tree *tree, Node *node) {
  // NOTE: Assumes a non-empty tree

  Node *current = tree->root;
  Node *above = NULL;
  char *str = node->word;
  int diff = strcmp(current->word, str); // if 1st < 2nd, will return negative
  while(current != NULL) {
    if(diff < 0) { 		// current string less than new string
      above = current;
      current = current->right;
    }
    else if(diff >= 0) { 	// current string less than or equal to new string
      above = current;
      current = current->left;
    }
    if(current != NULL){
      diff = strcmp(current->word, str);
    }
  }
  diff = strcmp(above->word, str);
  if(diff < 0) {
    // above->word < new string
    above->right = node;
    node->parent = above;
  } 
  else if(diff >= 0){
    // above->word >= new string
    above->left = node;
    node->parent = above;
  }
}

void rotateLeft(Tree *tree, Node *node) {
  // node is the parent in this case
  // giving node its child's left subtree
  Node *originalChild = node->right;
  node->right = originalChild->left;
  originalChild->left = node;
  
  //switching parents
  if(node->parent == NULL) { // if node is the root
    tree->root = originalChild;
  }
  else { // node is not the root (i.e. has a parent)
    // setting node's original parent's child to originalChild
    if(node->parent->left == node) { // i.e. node was left child of its parent
      node->parent->left = originalChild;
    }
    else { // node was right child of its parent
      node->parent->right = originalChild;
    }
  }
  Node *temp = node->parent;
  node->parent = originalChild;
  originalChild->parent = temp;
  if(node->right != NULL) {
    node->right->parent = node;
  }
}

void rotateRight(Tree* tree, Node *node) { // TODO must be same type
  // node is the parent in this case     // as in .h file
  // giving node its child's right subtree
  Node *originalChild = node->left;
  node->left = originalChild->right;
  originalChild->right = node;
  
  // switching parents
  if(node->parent == NULL){ // if node is the root
    tree->root = originalChild;
  }
  else { // node is not the root (i.e. has a parent)
    // setting node's original parent's child to originalChild
    if(node->parent->left == node) { // i.e. node was left child of its parent
      node->parent->left = originalChild;
    }
    else { // node was right child of its parent
      node->parent->right = originalChild;
    }
  }
  Node *temp = node->parent;
  node->parent = originalChild;
  originalChild->parent = temp;
  if(node->left != NULL) {
    node->left->parent = node;
  }
}

void insert(Tree *tree, Node *node){
  if(tree->root == NULL){ 	// Root is NULL
    tree->root = node;
    tree->root->col = BLACK;
  }
  else {		  	// Root is not NULL
    BSTinsert(tree, node);	// Do normal BST insert
    node->col = RED;		// Set color to RED (in case it wasn't
				// initialzed as such)
    
    // Color changes keep occurring as long as node and its parent
    // are both red (rotations can only happen 1 to 2 times at most)
    while((node != tree->root) && (node->parent->col == RED)) {

      // There shall always exist a grandfather at this point because there can
      // never be a red root and its children can never be red at the same time
      if(node->parent == node->parent->parent->left) {	// parent is left child

        Node *uncle = node->parent->parent->right;
        // case where uncle is black/NULL
        if(uncle == NULL || uncle->col == BLACK) {

          // check if grandfather and node are zigzag	//	G       /                  G      /
          if(node == node->parent->right) {		//     / \      /                 / \     /
            node = node->parent;			//    P  (U)    /  ---->>>>      N  (U)   /
            rotateLeft(tree, node);			//   / \   \    /               /         / 
          						//  A   N       /              P <-- node points here now
							//     / 	/	      / \         /
         						//    X	 	/	     A   X        /
          }						
          node->parent->col = BLACK;
          node->parent->parent->col = RED;
          rotateRight(tree, node->parent->parent);
        } // end if for black/NULL uncle
        
        // case where uncle is red
        else {
          // make uncle and parent black and grandparent red
          uncle->col = BLACK;
          node->parent->parent->col = RED;
          node->parent->col = BLACK;
          node = node->parent->parent;
        } // end else (for red uncle)
      } // end outer if (for when parent is left child)

      // node's parent is node's grandfather's right child
      else {
        Node *uncle = node->parent->parent->left;
        // case where uncle is black/null
        if(uncle == NULL || uncle->col == BLACK) {
          // check if grandfather and node are zigzag
          if(node == node->parent->left){
            node = node->parent;
            rotateRight(tree, node);
          }
          node->parent->col = BLACK;
          node->parent->parent->col = RED;
          rotateLeft(tree, node->parent->parent);
        } // end if
        // case where uncle is red
        else { // TODO factor out?
          uncle->col = BLACK;
          node->parent->parent->col = RED;
          node->parent->col = BLACK;
          node = node->parent->parent;
        } // end else
      } // end else
    } // end while
  } // end else
  tree->root->col = BLACK;
}

  

int rotTest() {
  Tree t; // TODO malloc it?
  initTree(&t); // if we don't include this, it doesn't work
  Node p, x, y, a, b, c;

  initNode(&p, BLACK, "cat");
  initNode(&x, RED, "dog"); // x on right of p
  initNode(&y, RED, "fat"); // y on right of x
  initNode(&a, RED, "cool"); // a on left of x
  initNode(&b, RED, "eel"); // b on left of y
  initNode(&c, RED, "horse"); // c on right of y

  insert(&t, &p);
  insert(&t, &x);
  insert(&t, &y);
  insert(&t, &a);
  insert(&t, &b);
  insert(&t, &c);


  rotateLeft(&t, &x);

  rotateRight(&t, &y);
  BSTinorder(t.root);
   // Expected output: cat cool dog eel fat horse 
  return 0;

}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Incorrect number of arguments!\n"
    "Usage: ./pointersorter \"[STRING]\"\n");
    return -1;
  }

  Tree t;
  initTree(&t);
  char *word;
  Node *node;
  char *curr = argv[1]; //points to the current word being parsed
  long long count = 0; //keeps track of the current word's length
  char letter = *curr; //keeps track of the current letter being iterated through
  long long beginningIndex = 0; //keeps track of where the first char is in a contiguous sequence of alphabetic letters
  long long currentIndex = 0; //keeps track of the index of the current letter being iterated on
  
  while (letter != '\0') { //keep looping until the end of the input
    // printf("Letter: %c\n", letter);
    if (isalpha(letter)) {
      count++;
    }
    else {
      if (count != 0) { //signifies the end of a consecutive sequence of alphabetic characters
        // printf("End of word found\n");
        word = (char *) malloc(sizeof(char) * (count + 1)); //+1 for null terminating character
        if (word == NULL) {
          printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
          return -1;
        }
        memcpy(word, argv[1] + beginningIndex, count);
        word[count] = '\0';
        count = 0;
        node = (Node *) malloc(sizeof(Node));
        if (node == NULL) {
          printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
          return -1;
        }
        initNode(node, RED, word);
        insert(&t, node);
      }
      else { // If count is 0 then that means two consecutive
	     //non alphabetic characters have been found in a row
      }

      beginningIndex = currentIndex + 1;
    }
    //update variables for next iteration
    //printf("%c", letter);
    curr++;
    currentIndex++;
    letter = *curr;
  }
  // In the case that a contiguous sequence of alphabetic letters
  // doesn't end with a non alphabetic char (i.e. its end is
  // the end of the string), it has to be added this way
  if (count > 0) {
    word = (char *) malloc(sizeof(char) * (count + 1)); //+1 for null terminating character
    if (word == NULL) {
      printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
      return -1;
    }
    memcpy(word, argv[1] + beginningIndex, count);
    word[count] = '\0';
    count = 0;
    node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
      printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
      return -1;
    }
    initNode(node, RED, word);
    insert(&t, node);
  }
  
  BSTinorder(t.root);
  //printTree(&t);
  freeNodesInTree(&t);
  
  return 0;
}

