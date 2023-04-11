*****************************
Name - Smrithi Pyari Lal
CS 370
Programming Project 2
Due date - 02/06/2022, 11:55pm
*****************************

PROJECT DESCRIPTION
For this assignment, we had to extend the isched structure to be able to keep count of the number of processes in the run queue and process queue. There were three things in particular that we had to do -
1. When process is added to either queues, print the number of processes in both queues
2. When a process is deleted from either queues, print the number of processes in both queues
3. When the number of elements in the run queue is 0, print the process counts of both queues.

LOGIC
To get to the desired result, I looked at the dis.c file and took note of all the places the queues were being modified. I could tell by looking at the way pointers of the linked list were being manipulated. For example, code like this would be adding a process to the end of the process queue. 

if(isched.tail != nil) {
      n->prev = isched.tail;
      isched.tail->next = n;
   }
   else {
      isched.head = n;
      n->prev = nil;
   }
   isched.tail = n;

Some things which I had to keep in mind - 
1. Process queues would be addressed as isched.head and isched.tail while run queues would be addressed as isched.runhd and isched.runtl.

2. Run queue count should always be less than process queue count as it is a subset of it. 

3. Finally when I exit, both process queue and run queue should have 0 processes.

Functions involved
Process queue- 
newprog, delprog
Run queue-
additions - addrun, acquire, pushrun
negations - delprog, delrun, delrunq

Testing
Finally to test my code I used simple commands like cd and ls to check if the counts were correct. I also made sure to print out the function name which was causing the count changes just to be sure. Once I saw the desired results from these tests I ran the test1.b limbo file provided on the course website to test if the process counts were correct and the file performed as expected.




 