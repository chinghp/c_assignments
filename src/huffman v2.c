#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<ctype.h>

#define ASCII 256
/*assume only ASCII codes of 0 to 255*/

struct node{
   int depth; /*depth in the tree*/
   int number; /*for the bottom node point to a pointer*/
   struct node *next; /*next node with value only*/
   struct node *left;
   struct node *right;
   int frequency;
};

typedef struct node Node;

struct result{
   int** binary_code;
   int* binary_size;
};

typedef struct result Result;

/*count frequency of individual ASCII code char in the file*/
int *count_frequency(FILE *fp);

/*create a linked list arranging ASCII code in ascending order by frequency*/
Node *ascending_order(int* frequency);

/*initialize a new node*/
Node *Allot_node(int i);

/*seek the appropriate position for linked list insertion*/
Node *insert_position(Node* start, Node* tmp);

/*build the appropriate ASCII code tree based on word frequency*/
Node *build_binary_tree(Node* start);

/*merge 2 nodes and create a new one*/
Node *merge_node(Node* temp1, Node* temp2);

/*function to initialize result array and run "intree" looping, output the binary values*/
Result *binary_array(Node* top_of_tree, int* frequency);

/*a loop to calculate binary code in tree*/
Result *intree(Node* top_of_tree, int* frequency, int*binary_code, int length, Result* result);

/*print part 1 result on screen*/
void printresult(int* frequency, int max_length, Result* result);

/*free necessary allocated memory in struct result*/
void freeresult(Result* result);

/*free necessary allocated memory in struct node*/
void freenode(Node *start);

int main(int argc, char **argv)
{

   FILE *fp;
   int* frequency;
   Node *start, *top_of_tree;
   Result* result;

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

   /*free*/
   free(frequency);
   freeresult(result);
   freenode(start);

	return 0;
}


int *count_frequency(FILE *fp){
   int tmp_word;
   int *frequency;

   frequency = (int*)calloc(ASCII, sizeof(int));

   /*start to read characters.*/
   while((tmp_word = getc(fp)) != EOF){
      frequency[(int)tmp_word]+=1;
   }

   return frequency;
}

/*the linked list include all chars with frequency>0*/
Node *ascending_order(int* frequency){
   int i;
   Node *start, *current, *tmp;
   start = NULL;


   for(i=0;i<ASCII;i++){

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

Node *merge_node(Node* temp1, Node* temp2){
   Node *new;
   new=Allot_node(0);
   new->frequency=temp1->frequency+temp2->frequency;
   new->left=temp1;
   new->right=temp2;

   if(temp1->depth>temp2->depth){
      new->depth = temp1->depth+1;
   }
   else{
      new->depth = temp2->depth+1;
   }

   return new;
}

Result* binary_array(Node* top_of_tree, int* frequency){
   Result* result;
   int *binary_code;

   /*initialize result*/
   result = (Result*)malloc(sizeof(Result));
   result->binary_code = (int**)calloc(ASCII,sizeof(int*));
   result->binary_size = (int*)calloc(ASCII,sizeof(int));

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

   for(i=0;i<ASCII;i++){
      if(frequency[i]){
         if(isprint((char)i)){
            printf("'%c'", (char)i);
         }
         else{
            printf("%03d", i);
         }
         printf(" : ");
         for(j=0;j<max_length-result->binary_size[i];j++){
            printf(" ");
         }
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

void freeresult(Result* result){
   int i;
   for(i=0; i<ASCII; i++){
      if(result->binary_code[i]!=NULL){
         free(result->binary_code[i]);
      }
   }
   free(result->binary_size);
   free(result);
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
