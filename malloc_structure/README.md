# Porject 3

For this project we had to replace the binary searvh tree of linked lists in inferno's memory allocation mechanism to a linked list in scending order. For this I made the following changes - 

Firstly I removed all of the definitions that were previously in place for the binary tree and had just one definition - #define next u.s.bhr

Then I noted the functions that I would need to change - pooldel, pooladd, dopoolalloc & poolmax. 

The easy changes were in dopoolalloc and poolmax so I'll start with those. 

## dopoolalloc - 

I changed all t->right to be t->next because in my version of the code, 'right' is not defined. Also, right would mean the right child of the tree which in my case would simply have next as it is a linked list. Also, one part of this function involved moving to the next block. In the case of a binary tree, we would have left and right children, but since that is no longer valid in a linked list, I simply did t= t->next.

## poolmax - 

Here too, I changed all t->right instances to t->next.

There was another function that required chaning variable names - poolcompact. Here, we were originally using a Bhdr structure called *next. But since we defined next to be u.s.bhrd, to avoid issues with that, I changed all references to next to othernext. 

Now for the part of code that rewuired chaning logic - 

## pooldel- 

In pooldel, we now needed to travers the linked list and find the block that needed to be removed and fix the pointers for in accordance to a linked list. There were three parts to this, first if there are no blocks in the pool, we simply return. Second, if the block to delete was the root, I changed the root to be p->root->next, this made the next block the root and essentially deleting the root block. Latsly, if the block is somewhere in the middle of the list, I had a while loop that runs till the block is found. I ha da temp variable and a current variable to keep track of the current and previous block while going over the list. When the block to delete is found, I make the next pointer of the previous block point to the next block of the current block. This way the previous blokc links to the next block and the current block gets deleted. If the block is not found, we simply return. 


## pooladd - 

In pooladd I had to check three conditions to correctly add the new block in its position. firstly, if there are no blocks in the pool, I simply made the root be the new block that i had to add. Second, if the block to add was smaller than the root fo the pool, I made the next pointer of the block point to the head of the list and set the root of the list to be the block that got added. Lastly, if I had to add the block somewhere in the middle of my linked list, I had a while loop traverse the list till either all blocks had been checked or we found a block with a size bigger than the size we were adding. When one of these conditions are hit, I set the next pointer of the block equal to the current blocks next block. This inserted the block between the current block and the block whos size is bigger than the one to be inserted. I then changed current->next = q. This inserted the block in the current spot. 

## Testing - 

For testing this code, I compiled and started the operating system and everything ran smoothly. I then ran test1.b to see if memory allocations were happening alright and I didnt see any errors, so I believe my implementation doesn't have errors. 


