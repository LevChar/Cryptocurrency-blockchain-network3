/* ======================================================== */
/*   linked_list.c		      								*/
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/linked_list.h"

//---------------------------------------------------------------------------
//---------------------- Private Methods Prototypes -------------------------
//---------------------------------------------------------------------------
Singly_Linked_Node 
*allocate_list_cell(pVoid i_data_ptr);

//---------------------------------------------------------------------------
//-----------------------Private Methods Implementations---------------------
//---------------------------------------------------------------------------
PRIVATE 
Singly_Linked_Node* 
allocate_list_cell(pVoid i_data_ptr)
{
	//S_L_N is a Typedef for Singly_Linked_Node
    S_L_N *cell = malloc(sizeof(S_L_N));
    if(NULL == cell)
    {
        printf("Memory allocation error!\n");
        exit(1);
    }

    cell->data = i_data_ptr;
    cell->next = NULL;

    return cell;
}

//---------------------------------------------------------------------------
//---------------------- Public Methods Implementations----------------------
//---------------------------------------------------------------------------
PUBLIC
Singly_Linked_List* 
initialize_Empty_List(size_t i_element_Size, freeFunction i_free_Fn)
{
	Singly_Linked_List*  new_list = malloc(sizeof(Singly_Linked_List));
	new_list->head = new_list->tail = NULL;
	new_list->element_size = i_element_Size;
	new_list->length = 0;
	new_list->freeFn = i_free_Fn;
	
	return new_list;
}
					  
PUBLIC
void 
append_To_List(Singly_Linked_List *o_list, pVoid i_element)
{
	Singly_Linked_Node *cell = allocate_list_cell(i_element);

	if(o_list->length == 0)
    {
        o_list->head = o_list->tail = cell;
    }
	else
    {
        o_list->tail->next = cell;
        o_list->tail = cell;
    }

  o_list->length++;
}

PUBLIC 
void
destroy_List(Singly_Linked_List *i_list)
{
	Singly_Linked_Node *current = i_list->head;

	while (current != NULL) 
    {
		i_list->head = current->next;
		if (i_list->freeFn) 
        {
			i_list->freeFn(current->data);
		}
		else
        {
            free(current->data);
		}

		free(current);
		i_list->length--;
		current = i_list->head;
	}
}

PUBLIC
pVoid
get_last_block(Singly_Linked_List *i_list)
{
    return i_list->tail->data;
}