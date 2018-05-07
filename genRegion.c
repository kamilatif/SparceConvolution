#include <stdio.h>
#include <stdlib.h> /*malloc*/
#include <time.h>
#include <stdbool.h> /*bool*/

#ifndef max
    #define max(a,b) ((a) >= (b) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b) ((a) <= (b) ? (a) : (b))
#endif

#define convlMatrix 9

clock_t start, end;
double cpu_time_used;

int testPivotArr[] = {8, 12, 36, 40, 10, 38, 24};

/* No. of a row = x-coordinate 
   No. of a col = y-coordinate */
void ImRegion( int A[], int pivot, int ImSize, int KerSize ) {
		int x_cord = pivot / ImSize;
		int y_cord = pivot % ImSize;

		//printf("Pivot is %d, X-Cord is %d and Y-Cord is %d\n", pivot, x_cord, y_cord);
		int upRow = x_cord - (KerSize/2);
		upRow = max(upRow, 0);
		int downRow = x_cord + (KerSize/2);
		downRow = min(ImSize-1, downRow);
		//printf("upRow is %d, downRow is %d \n", upRow, downRow);
		
		int leftCol = y_cord - (KerSize/2);
		leftCol = max(leftCol, 0);
		int rightCol = y_cord + (KerSize/2);
		rightCol = min(ImSize-1, rightCol);
		//printf("leftCol is %d, rightCol is %d \n", leftCol, rightCol);

		/*Writing the TOP row*/
		int tempCounter = 0;
		for(int i = (upRow*ImSize)+leftCol; i < (upRow*ImSize)+rightCol+1; i++) {
			 *(A + tempCounter) = i;
			 tempCounter+=1;
		}
		/*Writing the OTHER rows*/
		for(int i = 1; i < (downRow-upRow+1) ; i++) {
			// for(int j = KerSize; j < convlMatrix; j++) {
			// 		*(A + j+(i-1)*KerSize) = *(A + j-KerSize) + i*ImSize;
			// }
			*(A + 3+(i-1)*KerSize) = *(A + 0) + i*ImSize;
			*(A + 4+(i-1)*KerSize) = *(A + 1) + i*ImSize;
			*(A + 5+(i-1)*KerSize) = *(A + 2) + i*ImSize;
		}
}

/*Print Array*/
void printArray(int A[], int size) {
	for(int i = 0; i < size; i++) {
		if(i != 0 && i%3 == 0) { printf("\n");}
		printf("%d\t", A[i]);
	}
	printf("\n\n");
}

/*Region Structure*/
typedef struct regions {
 		int subImage[convlMatrix];
 		struct regions *next; 	
} region_t;


/*Insert Node at Front in LinkedList*/
region_t* insertNode(region_t *HeadNode) { 
	region_t *newNode = (region_t *) malloc(sizeof(region_t));
	newNode -> next = HeadNode -> next;
	HeadNode -> next = newNode;
	return newNode;
}

 /*Print Nodes*/
void printNodes(region_t const *HeaderForNode) {
	printf("*****REGIONS GENERATED*****\n");
	region_t *dummyPrintNode = HeaderForNode -> next;
	while(dummyPrintNode != NULL) {
		printArray(dummyPrintNode -> subImage, convlMatrix);
		dummyPrintNode = dummyPrintNode -> next;
	}
}

/*Pivot Stucture*/
typedef struct pivots {
	int genPivot; 
	struct pivots *next;
} pivot_t;

/*Insert Node at Front in LinkedList*/
pivot_t* insertPivotNode(pivot_t *HeadNode, int data) { 
	pivot_t *newNode = (pivot_t *) malloc(sizeof(pivot_t));
	newNode -> next = HeadNode -> next;
	newNode -> genPivot = data;
	HeadNode -> next = newNode;
	return newNode;
}

/*Print Pivots*/
void printPivots(pivot_t const *HeaderForPivots) {
	printf("Pivots = ");
	pivot_t *dummyPrintPivotNode = HeaderForPivots -> next;
	while((dummyPrintPivotNode) != NULL) {
		printf("%d ", (dummyPrintPivotNode ) -> genPivot);
		dummyPrintPivotNode = (dummyPrintPivotNode ) -> next;
	}
	printf("\n");
}

int main() {
	region_t *Head; 
	Head = (region_t *) calloc(1, sizeof(region_t));
	Head -> next = NULL;

	pivot_t *pivotHead;
	pivotHead = (pivot_t *) calloc(1, sizeof(pivot_t));
	pivotHead -> next = NULL;

	int interCounter = 0;
	bool nodeFound = 0;
	pivot_t *dummyPivotSearch = pivotHead;

	/*print 7x7 grid*/
	for( int i = 0; i < 49; i++) {
		printf("%d\t", i);
		if( (i+1) % 7 == 0)
			printf("\n");
	}
	printf("\n");


	for(int testIter = 0; testIter < 7; testIter++) {
		int pivot = testPivotArr[testIter];
		int tempArray[9] = {0};
		/*Skip the TOP ROW*/
		if((pivot / 7) == 0) {
			printf("Skipping sub-pivot (top ROW) = %d\n", pivot);
			continue;
		}
		/*Skip the LEFT ROW*/
		if((pivot % 7) == 0) {
			printf("Skipping sub-pivot (LEFT ROW) = %d\n", pivot);
			continue;
		}
		/*Skip the RIGHT ROW*/
		if((pivot % 7) == 6) {
			printf("Skipping sub-pivot (RIGHT ROW) = %d\n", pivot);
			continue;
		}
		/*Skip the BOTTOM ROW*/
		if((pivot / 7) == 6) {
			printf("Skipping sub-pivot (BOTTOM ROW) = %d\n", pivot);
			continue;
		}
		// Generate Regions for the main pivots
		// that are obtained from CSR file.
		ImRegion(tempArray, pivot, 7, 3);
		// For the first time, insert all generated points
		// to the linked list of pivots. Else, search backwarks. 
		if(interCounter == 0) {
			for(int i = 0; i < 9; i++) {
				//printf("Adding pivot = %d for Main Pivot = %d\n", tempArray[i], pivot);
				pivot_t *newNode = insertPivotNode(pivotHead, tempArray[i]);
			}
		} else {
			for(int i = 0; i < 9; i++) {
				nodeFound = 0;
				dummyPivotSearch = pivotHead;
				while(dummyPivotSearch -> next != NULL) {
					if((dummyPivotSearch -> next) -> genPivot == tempArray[i]) {
						//printf("Skipping pivot = %d for Main Pivot = %d\n", tempArray[i], pivot);
						nodeFound = 1;
						break;	
					} 
					dummyPivotSearch = (dummyPivotSearch -> next);	
				}
				if (!nodeFound) {
					//printf("Adding pivot = %d for Main Pivot = %d\n", tempArray[i],pivot);
					pivot_t *newNode = insertPivotNode(pivotHead, tempArray[i]);	
				}
			}
		}
		interCounter+=1;
	}

	/*Print All Pivots*/
 	printPivots(pivotHead);
	
	/*Generate regions for all the pivots*/
	pivot_t *pivotIter = pivotHead -> next;
	while(pivotIter != NULL) {
		
		//printf("Sub Pivot = %d\n", pivotIter -> genPivot);
		/*Skip the TOP ROW*/
		if(((pivotIter -> genPivot) / 7) == 0) {
			printf("Skipping sub-pivot (top ROW) = %d\n", pivotIter -> genPivot);
			pivotIter = pivotIter -> next;
			continue;
		}
		/*Skip the LEFT ROW*/
		if(((pivotIter -> genPivot) % 7) == 0) {
			printf("Skipping sub-pivot (LEFT ROW) = %d\n", pivotIter -> genPivot);
			pivotIter = pivotIter -> next;
			continue;
		}
		/*Skip the RIGHT ROW*/
		if(((pivotIter -> genPivot) % 7) == 6) {
			printf("Skipping sub-pivot (RIGHT ROW) = %d\n", pivotIter -> genPivot);
			pivotIter = pivotIter -> next;
			continue;
		}
		/*Skip the BOTTOM ROW*/
		if(((pivotIter -> genPivot) / 7) == 6) {
			printf("Skipping sub-pivot (BOTTOM ROW) = %d\n", pivotIter -> genPivot);
			pivotIter = pivotIter -> next;
			continue;
		}
		region_t *subNode = insertNode(Head);
		ImRegion(subNode -> subImage,  pivotIter -> genPivot, 7, 3);
		pivotIter = pivotIter -> next;
	}
	/*Print All Nodes*/	
	printNodes(Head);



	//	region_t *newNode = insertNode(Head);`
	//	printf("Main Pivot = %d\n", pivot);
	//	ImRegion(newNode -> subImage, pivot, 7, 3);
		

		

		// Copy pivots into temp array
		// for(int copyIndex = 0; copyIndex < 9; copyIndex++ ) {
		// 	tempArray[copyIndex] = newNode -> subImage[copyIndex];
		// }


		/*Generate Sub-regions*/
	// 	for(int iter = 0; iter < 9; iter++) {
	// 		/*Skip the Main pivot*/ 
	// 		if(newNode -> subImage[iter] == pivot) {
	// 			printf("Skipping sub-pivot (same as Main Pivot) = %d\n", newNode -> subImage[iter]);
	// 			continue;
	// 		}
	// 		/*Skip the TOP ROW*/
	// 		if(((newNode -> subImage[iter]) / 7) == 0) {
	// 			printf("Skipping sub-pivot (top ROW) = %d\n", newNode -> subImage[iter]);
	// 			continue;
	// 		}
	// 		/*Skip the LEFT ROW*/
	// 		if(((newNode -> subImage[iter]) % 7) == 0) {
	// 			printf("Skipping sub-pivot (LEFT ROW) = %d\n", newNode -> subImage[iter]);
	// 			continue;
	// 		}
	// 		Skip the RIGHT ROW
	// 		if(((newNode -> subImage[iter]) % 7) == 6) {
	// 			printf("Skipping sub-pivot (RIGHT ROW) = %d\n", newNode -> subImage[iter]);
	// 			continue;
	// 		}
	// 		/*Skip the BOTTOM ROW*/
	// 		if(((newNode -> subImage[iter]) / 7) == 6) {
	// 			printf("Skipping sub-pivot (BOTTOM ROW) = %d\n", newNode -> subImage[iter]);
	// 			continue;
	// 		}
	// 		region_t *subNode = insertNode(Head);
	// 		printf("Sub Pivot = %d\n", newNode -> subImage[iter]);
	// 		ImRegion(subNode -> subImage, newNode -> subImage[iter], 7, 3);
	// 		//printf("Iter=%d\t subPivot=%d\n",iter,(newNode -> subImage[iter]));
	// 	}
	// }

	//int subImage[convlMatrix] = {0};
	//start = clock();
	//end = clock();
	//cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("%f\n", cpu_time_used);

	//region_t *firstNode;
	//Head -> next = firstNode; //points to the header of the list
	//firstNode = (region_t *) malloc(sizeof(region_t));
	//firstNode -> next = NULL;
	//Head -> next = firstNode; //points to the header of the list
	//ImRegion(firstNode -> subImage, 10, 7, 3);
	//printf("Head Address is = %p\n", Head);
	//printf("firstNode Address is = %p\n", firstNode);

	//printArray(Head -> next -> subImage, convlMatrix);
	//printArray((Head -> next) -> next -> subImage, convlMatrix);
	//printArray(((Head -> next) -> next) -> next -> subImage, convlMatrix);
	//printArray((((Head -> next) -> next) -> next) -> subImage, convlMatrix);



	return 0;
} 