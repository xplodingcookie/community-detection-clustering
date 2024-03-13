/* A simplified community detection algorithm:
 *
 * Skeleton code written by Jianzhong Qi, May 2023
 * Edited by: Dong Li 1352898
 *
 */

/* Algorithms are fun!!! */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <string.h>

#define STAGE_NUM_ONE 1						  /* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */

#define MAX_NUM_USERS 50
#define MAX_NUM_HASHTAGS 10
#define MAX_LENGTH_HASHTAG 20
#define NOT_CORE_USER 0
#define IS_CORE_USER 1
#define HASHTAGS_PER_LINE 5

typedef struct {
	/* add your user_t struct definition */
	int user_id, year, num_hashtags;
	char hashtags[MAX_NUM_HASHTAGS][MAX_LENGTH_HASHTAG + 1];
	int close_friends[MAX_NUM_USERS], num_close_friends;
} user_t;

typedef char* data_t;			 /* to be modified for Stage 4 */

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
void print_list(list_t *list);
list_t *insert_unique_in_order(list_t *list, data_t value);

void print_stage_header(int stage_num);

void stage_one(user_t *user, int *num_users);
void stage_two(int friend_matrix[][MAX_NUM_USERS], int num_users);
void stage_three(int friend_matrix[][MAX_NUM_USERS],
					float connection_matrix[][MAX_NUM_USERS], int num_users);
void stage_four(user_t *user, int friend_matrix[][MAX_NUM_USERS], 
				float connection_matrix[][MAX_NUM_USERS], int num_users);

/* add your own function prototypes here */

float s_o_c(int friend_matrix[][MAX_NUM_USERS], int num_users, int user_id_1, int user_id_2);
void find_close_friend_info(user_t *user, float connection_matrix[][MAX_NUM_USERS],
							int num_users, float ths);
int is_core_user(int user_id, user_t *user, int thc);
void print_stage_4_1(user_t *user, int user_id);

/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
	/* add variables to hold the input data */
	user_t user[MAX_NUM_USERS];
	int friend_matrix[MAX_NUM_USERS][MAX_NUM_USERS];
	int num_users;
	float connection_matrix[MAX_NUM_USERS][MAX_NUM_USERS];

	/* stage 1: read user profiles */
	stage_one(user, &num_users); 
	
	/* stage 2: compute the strength of connection between u0 and u1 */
	stage_two(friend_matrix, num_users);
	
	/* stage 3: compute the strength of connection for all user pairs */
	stage_three(friend_matrix, connection_matrix, num_users);
	
	/* stage 4: detect communities and topics of interest */
	stage_four(user, friend_matrix, connection_matrix, num_users);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* stage 1: read user profiles */
void 
stage_one(user_t *user, int *num_users) {
	/* add code for stage 1 */
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	int n_users = 0, max_hashtag = 1, num_hashtags = 0, max_hastag_user = 0;

	/* scan in the user data */
	for(int i=0; scanf("u%d %d", &user[i].user_id, &user[i].year) == 2; i++) {
		num_hashtags = 0;
		int j = 0;
		/* scan the hashtags into the struct for the user */
		while(scanf(" #%s", user[i].hashtags[j]) == 1) {
			j++;
			num_hashtags++;
		} 
		/* check and update max hashtags; keep track of which user has max hashtags */
		if (num_hashtags > max_hashtag){
			max_hashtag = num_hashtags;
			max_hastag_user = i;
		}
		/* set num_hashtags argument in the user struct to the number of hashtags counted*/
		user[i].num_hashtags = num_hashtags;
		n_users++;
	}
	*num_users = n_users;

	/* print results */
	printf("Number of users: %d\n", *num_users);
	printf("u%d has the largest number of hashtags:\n", user[max_hastag_user].user_id);
	/* iterate through to print the hashtags, note: don't want whitespace at end */
	int i = 0;
	while (user[max_hastag_user].hashtags[i][0] && i < (max_hashtag - 1)) {
		printf("#%s ", user[max_hastag_user].hashtags[i]);
		i++;
	} 
	printf("#%s\n", user[max_hastag_user].hashtags[i]);
	printf("\n");
}

/* stage 2: compute the strength of connection between u0 and u1 */
void 
stage_two(int friend_matrix[][MAX_NUM_USERS], int num_users) {
	/* add code for stage 2 */
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);
	/* scan in entire friend ship matrix */
	for (int i = 0; i < num_users; i++) {
		for (int j = 0; j < num_users; j++) {
			scanf("%d", &friend_matrix[i][j]);
		}
	}
	/* print the strength of connection between u0 and u1 using s_o_c function */
	printf("Strength of connection between u0 and u1: %4.2f\n", 
							s_o_c(friend_matrix, num_users, 0, 1)); 

	printf("\n");
}

/* stage 3: compute the strength of connection for all user pairs */
void 
stage_three(int friend_matrix[][MAX_NUM_USERS], 
				float connection_matrix[][MAX_NUM_USERS], int num_users) {
	/* add code for stage 3 */
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);

	/* find and input the strength of connection(s_o_c) between two users into the 
	connection matrix */
	for (int i = 0; i < num_users; i++) {
		/* Note: we are just going to iterate through the upper triangle as the connection
		matrix is symmetric */
		for (int j = num_users; j >= i; j--) {
			float connection_lvl = s_o_c(friend_matrix, num_users, i, j);
			connection_matrix[i][j] = connection_lvl;
			connection_matrix[j][i] = connection_lvl;
		}
	}
	
	/* iterate through and print out contents of connection matrix */
	for (int i = 0; i < num_users; i++) {
		for (int j = 0; j < num_users; j++) {
			if (j == num_users - 1) {
				printf("%4.2f", connection_matrix[i][j]);
			} else {
				printf("%4.2f ", connection_matrix[i][j]);
			}
		} 
		printf("\n");
	} 
	printf("\n");
}

/* stage 4: detect communities and topics of interest */
void stage_four(user_t *user, int friend_matrix[][MAX_NUM_USERS], 
				float connection_matrix[][MAX_NUM_USERS], int num_users) {
	/* add code for stage 4 */
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);
	float ths;
	int thc;
	scanf("%f %d", &ths, &thc);

	/* add how many close friends and their user numbers to the user struct for	each user */
	find_close_friend_info(user, connection_matrix, num_users, ths);

	/* iterate through each user and check if they are core user */
	for (int user_id = 0; user_id < num_users; user_id++) {
		if (is_core_user(user_id, user, thc)) {
			/* print out the core user and all their close friends */
			print_stage_4_1(user, user_id);

			printf("Stage 4.2. Hashtags:\n");
			/* add all unique hashtags of core user and their close friends to linked list */
			list_t *list = make_empty_list();
			/* add the core user and core users friends, hashtags to linked list */
			for (int i = 0; i < user[user_id].num_hashtags; i++) {
				data_t hashtag = user[user_id].hashtags[i];
				list = insert_unique_in_order(list, hashtag);
			}
			for (int i = 0; i < user[user_id].num_close_friends; i++) {
				int close_friend = user[user_id].close_friends[i];
				for(int j = 0; j < user[close_friend].num_hashtags; j++) {
					data_t hashtag = user[close_friend].hashtags[j];
					list = insert_unique_in_order(list, hashtag);
				}
			}
			/* print the linked list and free it */
			print_list(list);
			free_list(list);
			printf("\n");
		}
	}
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t
*make_empty_list(void) {
	list_t *list;

	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;

	return list;
}

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
	node_t *curr, *prev;

	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(list);
}

/* insert a new data element into a linked list, keeping the data elements in
the list unique and in alphabetical order */
list_t
*insert_unique_in_order(list_t *list, data_t value) {
	/* the following code inserts a new node to the end of list.
		Modify it to suit Stage 4.2. Start by modifying it to 
		insert a new node while keeping an alphabetical order
		(think about how to insert in the middle of a list), 
		then, only insert when value is not in list already.
		[remove this comment to save line space if needed]
	*/

	node_t *new;

	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->data = value;
	new->next = NULL;

	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		/* iterate through list and find if its unique and if unique insert it alphabetically */
		node_t *curr, *prev = NULL;
		curr = list->head;
		while (curr && (strcmp(curr->data, value) < 0)) {
			prev = curr;
			curr = curr->next;
		}
		/* check whether it is unique, if not unique free new */
		if (curr != NULL && (strcmp(curr->data, value) == 0)) {
			free(new);
		} else {
			/* check where it is to be placed and insert it into list */
			if (prev == NULL) {
				/* insert at head of list */
				new->next = list->head;
				list->head = new;
			} else if (curr == NULL) {
				/* insert at end of list */
				list->foot->next = new;
				list->foot = new;
			} else {
				/* insert in the middle of list */
				prev->next = new;
				new->next = curr;
			}
		}
	}
	return list;
}

/* print the hashtags in a linked list; 5 hashtags per line */
void
print_list(list_t *list) {
	/* add code to print list */
	node_t *current;
	current = list->head;
	int count = 0;

	/* iterate through linked list and print 5 hashtags per line */
	while (current) {
		/* Note: we dont want whitespace at end so have condition: current->next != NULL */
		if (current->next != NULL) {
			printf("#%s", current->data);
			current = current->next;
			count++;

			/* if 5 hashtags per line have been reached print newline */
			if (count % HASHTAGS_PER_LINE == 0) {
				printf("\n");
			} else {
				printf(" ");
			}
		} else {
			printf("#%s", current->data);
			current = current->next;
		}
	}
}

/* calculates the strength of connection between two users */
float s_o_c(int friend_matrix[][MAX_NUM_USERS], int num_users, int user_id_1, int user_id_2) {
	/* create arrays of the row in the friend matrix for each respective user */
	int u_1[num_users], u_2[num_users];
	for (int i = 0; i < num_users; i++) {
		u_1[i] = friend_matrix[user_id_1][i];
		u_2[i] = friend_matrix[user_id_2][i];
	}

	/* if not friends just return 0 */
	if (friend_matrix[user_id_1][user_id_2] != 1) {
		return 0;
	} else {
		/* find intersection */
		int num_intersection = 0;
		for (int i = 0; i < num_users; i++) {
			if ((u_1[i] == 1) && (u_2[i] == 1)) {
				num_intersection++;
			}
		}

		/* find union */
		int num_union = 0;
		for (int i = 0; i < num_users; i++) {
			if (u_1[i] == 1 || u_2[i] == 1) {
				num_union++;
			}
		}
		/* return the strength of connection */
		return ((float) num_intersection) / num_union;
	}
}

/* adds how many close friends and what user number they are to the user struct for each user */
void find_close_friend_info(user_t *user, float connection_matrix[][MAX_NUM_USERS],
							int num_users, float ths) {
	/* iterate through each user */
	for (int i = 0; i < num_users; i++) {
		int number_close_friends = 0, idx = 0;

		/* calculate whether user i and user j are "close friends" */
		for (int j = 0; j < num_users; j++) {
			if (connection_matrix[i][j] > ths) {
				number_close_friends++;
				/* add that user to close_friends */
				user[i].close_friends[idx] = j;
				idx++;
			}
		}
		user[i].num_close_friends = number_close_friends;
	}
}

/* Takes in a user number returns 1 if user is core user 0 if not */
int is_core_user(int user_id, user_t *user, int thc) {
	if (user[user_id].num_close_friends > thc) {
		return IS_CORE_USER;
	} else {
		return NOT_CORE_USER;
	}
}

/* prints the results for stage 4.1 */
void print_stage_4_1(user_t *user, int user_id) {
	printf("Stage 4.1. Core user: u%d; ", user_id);
	printf("close friends: ");
	for (int i = 0; i < user[user_id].num_close_friends; i++) {
		if (i == user[user_id].num_close_friends - 1) {
			printf("u%d\n", user[user_id].close_friends[i]);
		} else {
			printf("u%d ", user[user_id].close_friends[i]);
		}
	}
}


/****************************************************************/
/*
	Write your time complexity analysis below for Stage 4.2, 
	assuming U users, C core users, H hashtags per user, 
	and a maximum length of T characters per hashtag:

	At the beginning of stage 4 the connection matrix is iterated through to find
	the number of close friends a user has and who those friends are, this would 
	take O(U^2) time to complete. 
	
	Following this we can now look at the algorithm	implemented for stage 4.2.
	In the worst-case scenario all users are core users	and all users are close 
	friends with each other. So we first iterate through each check if they are
	a core user, O(U), then we iterate through each core user and their close 
	friends, O(C*U), as all users are friends of core user in worse-case. Following 
	this we then insert the hastags, H,	that have maximum length T, into the ordered 
	linked list. Worse-case scenario all the hashtags are unique so iterating through 
	the and then inserting into the linked list will take O(U^2 * H * T).
	Thus we get a time complexity of O(U*C*U*U^2*H*T) => O(C*U^4*H*T).

	Therefore the overall time complexity needed for the algorithm is O(U^2) + O(C*U^4*H*T).

*/
