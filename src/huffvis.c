#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<ctype.h>

#define LETTERS 26
/*only build tree for 26 letters*/

#define FIRSTASCIILETTER 65

struct node{
   int depth; /*depth in the tree*/
   int number; /*for the bottom node point to a pointer*/
   struct node *next; /*next node with value only*/
   struct node *left;
   struct node *right;
   int frequency;
   int count_zero;
   int count_one;
};

typedef struct node Node;

struct result{
   int** binary_code;
   int* binary_size;
};

typedef struct result Result;

struct part2tree{
   char** result;
   /*x and y coordinates are used for filling in the output tree*/
   int x_coordinates;
   int y_coordinates;
};

typedef struct part2tree Part2tree;

/*count frequency of individual letters in the file*/
int *count_frequency(FILE *fp);

/*create a linked list arranging letters in ascending order by frequency*/
Node *ascending_order(int* frequency);

/*initialize a new node*/
Node *Allot_node(int i);

/*seek the appropriate position for linked list insertion*/
Node *insert_position(Node* start, Node* tmp);

/*build the appropriate letter tree based on word frequency*/
Node *build_binary_tree(Node* start);

/*merge 2 nodes and create a new one*/
Node *merge_node(Node* left_node, Node* right_node);

/*function to initialize result array and run "intree" looping, output the binary values*/
Result *binary_array(Node* top_of_tree, int* frequency);

/*a loop to calculate binary code in tree*/
Result *intree(Node* top_of_tree, int* frequency, int*binary_code, int length, Result* result);

/*print part 1 result on screen*/
void printresult(int* frequency, int max_length, Result* result);

/*initialize the output array for filling in and start the fill tree loop*/
Part2tree *build_part2_tree(Node* top_of_tree);

/*start to fill the output array*/
Part2tree *filltree(Node* current, Part2tree* part2tree);

/*add a new branch if it is a '*', there are still subranch*/
Part2tree *addbranch(Node* current, Part2tree* part2tree, int direction);

/*output part 2 result to screen*/
void print_part2_tree(Part2tree *part2tree, Node* top_of_tree);

/*free necessary allocated memory in struct result*/
void freeresult(Result* result);

/*free necessary allocated memory in struct node*/
void freenode(Node *start);

/*free necessary allocated memory in struct part2tree*/
void free_part2tree(Part2tree *part2tree, Node* top_of_tree);

int main(int argc, char **argv)
{

   FILE *fp;
   int* frequency;
   Node *start, *top_of_tree;
   Result* result;
   Part2tree* part2tree;

   /*input 1 file only*/
   if(argc !=2){
      fprintf(stderr, "The inputs are not correct.\n");
      exit(2);
   }

	/*check file open*/
   fp = fopen(argv[1], "r");
   if(fp == NULL){
      fprintf(stderr, "Failed to open file.\n");
      exit(1);
   }


   frequency = count_frequency(fp);

   fclose(fp);

   start = ascending_order(frequency);

   top_of_tree = build_binary_tree(start);

   result = binary_array(top_of_tree, frequency);

   printresult(frequency, top_of_tree->depth, result);

   /*start part 2*/
   part2tree = build_part2_tree(top_of_tree);

   print_part2_tree(part2tree, top_of_tree);

   /*free*/

   free_part2tree(part2tree, top_of_tree);

   free(frequency);

   freeresult(result);

   freenode(start);

	return 0;
}


int *count_frequency(FILE *fp){
   int tmp_word;
   int *frequency;

   frequency = (int*)calloc(LETTERS, sizeof(int));

   /*start to read characters.*/
   while((tmp_word = getc(fp)) != EOF){
      if(isalpha(tmp_word)){

         tmp_word = toupper(tmp_word);
         frequency[tmp_word-FIRSTASCIILETTER]+=1;
      }
   }

   return frequency;
}

/*the linked list include all chars with frequency>0*/
Node *ascending_order(int* frequency){
   int i;
   Node *start, *current, *tmp;
   start = NULL;


   for(i=0;i<LETTERS;i++){

      if(frequency[i]!=0){
         if(start==NULL){
            start = current = Allot_node(i);
            current->frequency = frequency[i];
         }
         else{
            /*the node should be in ascending order*/

            tmp = Allot_node(i);

            tmp->frequency = frequency[i];

            if(tmp->frequency<start->frequency){
               tmp->next=start;
               start=tmp;
            }
            else{
               current = insert_position(start, tmp);

               tmp->next = current->next;
               current->next = tmp;
            }
         }
      }
   }

   return start;
}

Node *Allot_node(int i){
   Node* new;
   new = (Node*)malloc(sizeof(Node));

   if(new==NULL){
		fprintf(stderr, "Cannot Allocate Node\n");
		exit(3);
	}

   new->depth=0;
   new->number=i;
   new->next=NULL;
   new->left=NULL;
   new->right=NULL;
   new->frequency=0;
   new->count_zero=0; /*for building tree*/
   new->count_one=0; /*for building tree*/

   return new;
}

Node *insert_position(Node* start, Node* tmp){
   int stop=0;
   Node *thenode;
   thenode = start;

   while(thenode->next!=NULL&&!stop){
      if(tmp->frequency<thenode->next->frequency){
         stop=1;
      }
      else{
         thenode = thenode->next;
      }
   }
   return thenode;
}

Node *build_binary_tree(Node* start){
   Node *temp1, *temp2, *current, *new;
   current = start;

   while(current->next!=NULL){
      temp1 = current;
      temp2 = current->next;
      new = merge_node(temp2, temp1);

      if(temp2->next==NULL){
         /*no further elements*/
         temp2->next=new;
         current=new; /*i.e. current->next=NULL*/
      }
      else if(new->frequency<temp2->next->frequency){
         /*new one is less frequent than next one*/
         new->next = temp2->next;
         current = temp2->next = new;
      }
      else{
         /*insert new one to correct place*/
         current = temp1 = temp2->next;
         temp2 = insert_position(current, new);
         new->next = temp2->next;
         temp2->next = new;
      }
   }
   return current; /*i.e. top of tree*/
}

Node *merge_node(Node* left_node, Node* right_node){
   Node *new;
   new=Allot_node(0);
   new->frequency=left_node->frequency+right_node->frequency;
   new->left=left_node;
   new->right=right_node;

   /*depth is for creating binary node*/
   if(left_node->depth>right_node->depth){
      new->depth = left_node->depth+1;
   }
   else{
      new->depth = right_node->depth+1;
   }

   /*count_zero, count_one is for building tree*/
   if(left_node->count_zero<right_node->count_zero){
      new->count_zero = right_node->count_zero;
   }
   else{
      new->count_zero = left_node->count_zero+1;
   }

   new->count_one = left_node->count_one + right_node->count_one + 1;

   return new;
}

Result* binary_array(Node* top_of_tree, int* frequency){
   Result* result;
   int *binary_code;

   /*initialize result*/
   result = (Result*)malloc(sizeof(Result));
   result->binary_code = (int**)calloc(LETTERS,sizeof(int*));
   result->binary_size = (int*)calloc(LETTERS,sizeof(int));

   /*initialize binary_code*/
   binary_code = (int*)calloc(top_of_tree->depth,sizeof(int));

   result = intree(top_of_tree, frequency, binary_code, 0, result);
   /*0 indicates the top, the value indicates length to represent to node*/

   free(binary_code);
   return result;
}

Result* intree(Node* parent, int* frequency, int*binary_code, int length, Result* result){
   int i;
   if(parent->left==NULL){
      /*reached bottom node*/
      result->binary_code[parent->number]=(int*)malloc(length*sizeof(int));
      for(i=0; i<length; i++){
         result->binary_code[parent->number][i]=binary_code[i];
      }
      result->binary_size[parent->number]=length;
   }
   else{
      binary_code[length]=0;
      intree(parent->left, frequency, binary_code, length+1, result);
      binary_code[length]=1;
      intree(parent->right, frequency, binary_code, length+1, result);
   }

   return result;
}

void printresult(int *frequency, int max_length, Result* result){
   int i,j;
   long int bytes=0;

   for(i=0;i<LETTERS;i++){
      if(frequency[i]){
         printf("'%c'", (char)(i+FIRSTASCIILETTER));
         printf(" : ");
         /*print necessary blank spaces first*/
         for(j=0;j<max_length-result->binary_size[i];j++){
            printf(" ");
         }
         /*print result string*/
         for(j=0;j<result->binary_size[i];j++){
            printf("%d", result->binary_code[i][j]);

         }
         printf(" (%5d *%5d)\n", result->binary_size[i], frequency[i]);


         bytes=bytes+result->binary_size[i]*frequency[i];
      }
   }
   printf("%ld bytes\n", bytes);
   return;
}

Part2tree *build_part2_tree(Node* top_of_tree){
   Part2tree* part2tree;
   int i;

   /*initialize part2tree*/

   part2tree = (Part2tree*)malloc(sizeof(Part2tree));
   part2tree->result = (char**)calloc(1+2*top_of_tree->count_zero, sizeof(char*));
   for(i=0; i<(1+2*top_of_tree->count_zero); i++){
      part2tree->result[i] = (char*)calloc(1+2*top_of_tree->count_one, sizeof(char));
   }

   part2tree->x_coordinates=0;
   part2tree->y_coordinates=0;

   part2tree=filltree(top_of_tree, part2tree);

   return part2tree;
}

Part2tree *filltree(Node* current, Part2tree* part2tree){
   if(current->left==NULL){
      part2tree->result[part2tree->x_coordinates][part2tree->y_coordinates]=(char)(current->number+FIRSTASCIILETTER);
   }
   else{
      part2tree->result[(part2tree->x_coordinates)][part2tree->y_coordinates]='*';
      /*add left branch*/
      part2tree = addbranch(current, part2tree, 0);
      /*add right branch*/
      part2tree = addbranch(current, part2tree, 1);
   }

   return part2tree;
}

Part2tree *addbranch(Node* current, Part2tree* part2tree, int direction){
   /*direction, 0 for left node(down), 1 for right node(right)*/
   /*for vertical, only 1 | is needed*/
   int inserttimes, i;
   char insertchar;

   if(direction){
      insertchar = '-';
      inserttimes = 2*((current->count_one)-(current->right->count_one))-1;
      part2tree->y_coordinates+=1;
   }
   else{
      insertchar = '|';
      inserttimes = 1;
      part2tree->x_coordinates+=1;
   }

   /*insertchar based on number of lines needed*/
   for(i=0;i<inserttimes;i++){
      part2tree->result[(part2tree->x_coordinates)][part2tree->y_coordinates]=insertchar;
      if(direction){
         part2tree->y_coordinates+=1;
      }
      else{
         part2tree->x_coordinates+=1;
      }
   }

   /*run fill tree, then return x or y to original position*/
   if(direction){
      part2tree = filltree(current->right, part2tree);
      part2tree->y_coordinates-=inserttimes+1;
   }
   else{
      part2tree = filltree(current->left, part2tree);
      part2tree->x_coordinates-=inserttimes+1;
   }

   return part2tree;
}


void print_part2_tree(Part2tree *part2tree, Node *top_of_tree){
   int i,j;

   printf("\nThe tree is as follows:\n");

   for(i=0;i<(1+2*top_of_tree->count_zero);i++){
      for(j=0;j<(1+2*top_of_tree->count_one);j++){
         if(part2tree->result[i][j]==0){
            part2tree->result[i][j]=' ';
         }
         printf("%c", part2tree->result[i][j]);
      }
      printf("\n");
   }

   return;
}



void freeresult(Result* result){
   int i;
   for(i=0; i<LETTERS; i++){
      if(result->binary_code[i]!=NULL){
         free(result->binary_code[i]);
      }
   }
   if(result->binary_size!=NULL){
      free(result->binary_size);
   }
   if(result!=NULL){
      free(result);
   }
   return;
}

void freenode(Node *start){
   Node *temp1, *temp2;
   temp1=start;
   temp2=temp1->next;
   free(temp1);
   if(temp2->next!=NULL){
      freenode(temp2);
   }
   return;
}


void free_part2tree(Part2tree* part2tree, Node* top_of_tree){
   int i;
   Part2tree* temp;
   for(i=0;i<(1+2*top_of_tree->count_zero);i++){
      temp = part2tree;
      free(temp->result[i]);
   }
   temp=part2tree;
   free(temp->result);
   free(part2tree);

   return;
}
