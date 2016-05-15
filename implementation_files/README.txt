					****************************************************************	
								Record Manager
 					****************************************************************

Record Manager acts as an interface for record operations on page file through buffer manager.
We can insert records, delete records, update records, and scan through the records in a table.
A scan is associated with a search condition and only returns records that match the search condition. 
Each table is stored in a separate page file and the record manager should access the pages of the file through the buffer manager implemented in the last assignment.

*****************************************
			Team Members
	
1.Smriti Raj(Team Representative)
CWID: 20364719

2.Praveen Kumar Selavn
CWID:20351349

3.Vijay Bharrathi
CWID:20356386

4.Dhayalini Nagaraj
CWID:20359686

************** CONTENTS *****************
    * Source Files
    * MakeMe File
    * ReadMe

******* INSTRUCTIONS TO RUN **************

1)Instructions to run the code
------------------------------

a)For executing Record Manager

1) In the terminal,navigate to the file directory.

2) Type: 
	make -f makefile.mk

3) ./record_mgr

******* FUNCTIONS IMPLEMENTED ************

******************************************
	NAME : 		initRecordManager
	ARGUMENTS : void *mgmtData
	PURPORSE :  Initialize Record manager
	RETURNS : 	returns RC_OK if successfull
				
******************************************
	
******************************************
	NAME : 		shutdownRecordManager
	PURPORSE :  Shutdowns Recordmanager
	RETURNS : 	returns RC_OK if successfull
				
******************************************
	
******************************************
	NAME : 		createTable
	ARGUMENTS :char *name, Schema *schema
	PURPORSE :  Create a new table along with initializing the bufferpool.
	RETURNS : 	returns RC_OK if successfull
				returns following error codes if checks fails 
				-RC_FILE_NOT_FOUND;  
******************************************
	
******************************************
	NAME : 		openTable
	ARGUMENTS : RM_TableData *rel, char *name
	PURPORSE :  All operations on a table such as scanning or inserting records require the table to be opened first
	RETURNS : 	returns RC_OK if successfull
				
******************************************
	
******************************************
	NAME : 		closeTable
	ARGUMENTS : RM_TableData *rel
	PURPORSE :  Close the table and return a success message upon success.
	RETURNS : 	returns RC_OK if successfull
				
******************************************
	
******************************************
	NAME : 		deleteTable
	ARGUMENTS : char *name
	PURPORSE :   Destroys the particular table by calling destroy page function.
	RETURNS : 	returns RC_OK if successfull
				
******************************************
	
******************************************
	NAME : 		getNumTuples
	ARGUMENTS : RM_TableData *rel
	PURPORSE :  Gets number of tuples/records from the management data.
	RETURNS : 	returns number of Tuples
******************************************
	
******************************************
	NAME : 		insertRecord
	ARGUMENTS : RM_TableData *rel, Record *record
	PURPORSE :  Inserts new record into the table.The pagesize is incremented and the value to the new record is added.The particular 			  record as dirty as it was recently modified and update the content in the memory.
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		deleteRecord
	ARGUMENTS : RM_TableData *rel, RID id
	PURPORSE :  Deletes a record from table
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		updateRecord
	ARGUMENTS : RM_TableData *rel, Record *record
	PURPORSE : Writes updated record to table. If there is a primary key defined for the table, then uses checkIfPKExists() to check 			  if the new key violates primary key constraint.
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		getRecord
	ARGUMENTS : RM_TableData *rel, RID id, Record *record
	PURPORSE :  Reads a record from table
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		startScan
	ARGUMENTS : RM_TableData *rel, RM_ScanHandle *scan, Expr *cond
	PURPORSE :  Scans through the page file to fetch the desired records. If there is a scan condition specified, then only records 			satisfying the condition are added to the result set. If there is no scan condition, then all the records are 				added to the result set. Result setis stored in the scan handle which is later iterated over using next(). 
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		next
	ARGUMENTS : RM_ScanHandle *scan, Record *record
	PURPORSE :  Iterates over the result set of the scan. Returns current record and moves on to the next record in the result set.
	RETURNS : 	returns RC_OK, RC_RM_NO_MORE_TUPLES if successfull
******************************************
	
******************************************
	NAME : 		closeScan
	ARGUMENTS : RM_ScanHandle *scan
	PURPORSE : Closes the scan by destroying the result set and the scan handle. There should be a closeScan() corrresponsing to each 				startScan()
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		getRecordSize
	ARGUMENTS : Schema *schema
	PURPORSE :  The condition for the total number of attributes and datatype is checked. 
	RETURNS : 	returns Offsetattributes.
			returns following error codes if checks fails 
			-RC_INVALID_HANDLE	

******************************************
	
******************************************
	NAME : 		createSchema
	ARGUMENTS : int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys
	PURPORSE :  Creates and returns the schema handle for client's use. Accepts the table skeleton along with the primary key details 				and returns a schema strcuture populated with given details.
	RETURNS : 	returns schemaMemory
******************************************
	
******************************************
	NAME : 		freeSchema
	ARGUMENTS : Schema *schema
	PURPORSE :  Schema is made free
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		createRecord
	ARGUMENTS : Record **record, Schema *schema
	PURPORSE :   It creates a new record
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		freeRecord
	ARGUMENTS : Record *record
	PURPORSE : A record is made free by calling free function.
	RETURNS : 	returns RC_OK if successfull
******************************************
	
******************************************
	NAME : 		getAttr
	ARGUMENTS : Record *record, Schema *schema, int attrNum, Value **value
	PURPORSE :  Stores the value from pointer and checks for the datatype and increases datatype value accordingly and returns the 				value.
	RETURNS : 	returns RC_OK if successfull
			returns following error codes if checks fails 
			-RC_INVALID_DATATYPE	
******************************************
	
******************************************
	NAME : 		setAttr
	ARGUMENTS : Record *record, Schema *schema, int attrNum, Value *value
	PURPORSE :  To set the values
	RETURNS : 	returns RC_OK if successfull
			returns following error codes if checks fails 
			-RC_INVALID_DATATYPE	

Additional Functions Implemented:
	
	
	NAME : 		Checkslot
	ARGUMENTS : RID *var,tableDetails *t
	PURPORSE :  checks the slot value 
	RETURNS : 	Null
	
	
	NAME : 		AddPageInfo
	ARGUMENTS : RID *var,tableDetails *t
	PURPORSE :  Assigns page information and Record info
	RETURNS : 	Null
	
	NAME : 		AddEmptySpace
	ARGUMENTS : char *value, int sizeOfRecord
	PURPORSE :  Adds Empty Space 
	RETURNS : 	Null
	
	NAME : 		attributes
	ARGUMENTS : Schema *schema, int attrNum, int *final
	PURPORSE :  Function to set value based on the type of data
	RETURNS : 	return RC_OK
			
	NAME : 		dirtyandunpin
	ARGUMENTS : tableDetails *t
	PURPORSE :  Marks dirty and unpins page in BufferMgr
	RETURNS : 	Null
			
			
***********************************************************			
Additional Error Codes Added:

#define RC_ERROR 5
#define RC_ALGORITHM_NOT_IMPLEMENTED 6
#define RC_PINNED_PAGES_STILL_IN_BUFFER 7
#define RC_INVALID_DATATYPE 8
#define RC_FILE_DELETE_FAILED 9
#define RC_FILE_HANDLE_NOT_INITIATED 10
#define RC_PAGENUM_INVALID 11
#define RC_FILE_CANNOT_BE_CLOSED 12
#define RC_INVALID_PAGE_FILE_NAME 13


#define RC_FILE_CANNOT_BE_CREATED 304
#define RC_BM_BUFFERPOOL_INIT_FAILED 305
#define RC_BUFFER_PROB 308
#define RC_PIN_PAGE_FAILED 306
#define RC_INVALID_HANDLE 307





