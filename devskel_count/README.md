# PROJECT 4

For this assignment we had to make a file server that serves one file - skeldata
and increments a counter when the file is read and resets the counter to the value written to the file when a write operation is performed. 

## Approach

I made changes to the devskel.c file under inferno-os/emu/port/devskel.c

This is the file server and it is accessed using '#S'. I added skel under the dev structure in root and checked to see that my &skeldevtab appeared after doing an mk. 

For the main logic of implementing a counter, I used a static integer to hold the count values and a flag to check if the file has been read already or not. 

## skelread - 

Under skelread, I incrementing the counter and displaying it was failry trivial. I used the readnum function from dev.c to print out my number, however I noticed that my count was incrementing twice.
This meant that my read function was getting called twice. I figured this was because my function was getting read once to know the size of the file and read again to know the contents of the file. In both cases, my count was getting incremented and overall I had count values like 2, 4, 6... for each time I called cat '#S/skeldata'

To overcome this, I used a flag variable to see if my count was alraedy read once and if so, incrment the counter only after it has already been read. If not, set the flag to 1 and wait for the second read call to print the counter. 

## skelwrite - 

Under skelwrite, I implemented resetting the counter to the value that was written into the file. For example, if I did echo 42 > '#S/skeldata', my program would print and return the value 42. Then in the next read call, such as when I did cat '#S/skeldata', my output would be 43 meaning the counter got set to the value that was entered. 

If a non-numeric value gets interest, such an alphabet, the counter resets to 0. 

Lastly, since I only had one file to serve, I did one check to see if the request was for directory or not. If the user was requesting a directory, the devdirread(c, va, count, skeltab, nelem(skeltab), devgen); call would be made. If not, then the above skelread and skelwrite functions would make the necessary changes to the counter. 
