#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<unistd.h>
#include"storage_mgr.h"
#include"buffer_mgr.h"
#include"record_mgr.h"
#include"tables.h"



typedef struct tableDetails
{
    int noOfTuples;	          
    int slotId;                      
    int slotsEmpty;	          
    char *name;                   
    BM_PageHandle pageHandler;	  
    BM_BufferPool bp;	  
}tableDetails;

typedef struct ScanInfo
{
    Expr *condition;
    int numTuplesScanned;
    RID currentRow;                     
    BM_PageHandle pageHandler;   
}ScanInfo;

typedef struct variables
{
int slots;
int attributes;
int recordSize;
int fsetAttributes;
int pos;
int value;
int numOfPages;
int strLength;
int pageLength;
}variables;

SM_FileHandle smfileHandler;
SM_PageHandle smpageHandler;
tableDetails *t;
RID *var;
variables tempVar;
char *pointer;
char *fact;
char *newFact;

Schema storageSchema;
RC initRecordManager (void *mgmtData)
{
return RC_OK;
}

RC shutdownRecordManager ()
{
return RC_OK;
}



RC createTable (char *name, Schema *schema)
{

t= ((tableDetails *) malloc (sizeof(tableDetails)));
int n=4096;                                                                  
char size[n];                                                            
char *pageHandler = size;                                                 
memset(pageHandler, 0, n);                                               
tempVar.value=0;
while(tempVar.value<=sizeof(char))                                               
{                                                                           
*(int*)pageHandler = tempVar.value;                                              
pageHandler += 4;                                              
tempVar.value+=1;                                                                 
}                                                                           
*(int*)pageHandler = 3;                                                         
pageHandler += 4;                                             
while(tempVar.value<=sizeof(char))                                               
{                                                                           
*(int*)pageHandler = (int)schema->dataTypes[tempVar.value];                      
pageHandler +=sizeof(char);                                  
*(int*)pageHandler = (int) schema->typeLength[tempVar.value];                    
pageHandler +=sizeof(char);                                   
tempVar.value=tempVar.value+1;                                                                 
}                                                                           

createPageFile(name);                                                      
if(!name == NULL)                                                            
{  
	openPageFile(name, &smfileHandler);                                            
	writeBlock(0, &smfileHandler, size);                                                                        
                                                 
}
else                                                                        
{
     return RC_FILE_NOT_FOUND;                                      
}
initBufferPool(&t->bp, name, 50, RS_FIFO, NULL);   
return RC_OK;
}


RC openTable (RM_TableData *rel, char *name)
{

rel->mgmtData = t;                                                    
pinPage(&t->bp, &t->pageHandler, -1);                   
smpageHandler = (char*) t->pageHandler.data;                                 
t->noOfTuples= *(int*)smpageHandler;                                         
smpageHandler+=4;                                               
t->slotsEmpty= *(int*)smpageHandler;                                        
smpageHandler +=4;     
tempVar.value=0;                                                                   
tempVar.attributes = *(int*)smpageHandler;                                               
smpageHandler +=4;                                                

Schema *storageSchema;                                                         
storageSchema= (Schema*) malloc(sizeof(Schema));                               
storageSchema->numAttr= tempVar.attributes;                                        
storageSchema->attrNames= (char**) malloc(sizeof(char*)*3);                    
storageSchema->dataTypes= (DataType*) malloc(sizeof(int)*3);                   
storageSchema->typeLength= (int*) malloc(sizeof(int)*3);    
                                                            
while(tempVar.value <= sizeof(char))                                             
{                                                                           
	storageSchema->attrNames[tempVar.value]=(char*)malloc(sizeof(char)+sizeof(int));  
	tempVar.value+=1;                                                                 
}                                                                           
while(tempVar.value <= (sizeof(char) + sizeof(int)))                             
{                                                                           
	storageSchema->dataTypes[tempVar.value]= *(int*)smpageHandler;                        
	rel->schema = storageSchema;                                                 
	tempVar.value+=1;                                                                 
}                                                                           
unpinPage(&t->bp, &t->pageHandler);                     
return RC_OK;    
}



RC closeTable (RM_TableData *rel)
{
BM_BufferPool *bp=(BM_BufferPool *)rel->mgmtData;
tableDetails *t = rel->mgmtData;                                   
shutdownBufferPool(&t->bp);	                            
return RC_OK;
}


RC deleteTable (char *name)
{
destroyPageFile(name);	                                                  
return RC_OK;
}


int getNumTuples (RM_TableData *rel)					    
{
return(((tableDetails*)rel->mgmtData)->noOfTuples);
}


RC insertRecord (RM_TableData *rel, Record *record)			    
{					    
var = &record->id;		     
t = rel->mgmtData;  
//Assign Page and Record information             
AddPageInfo(var,t); 
tempVar.recordSize = getRecordSize(rel->schema);	 
var->slot = AddEmptySpace(fact, tempVar.recordSize);	
//Check Slot and Perform Operation
Checkslot(var,t);
markDirty(&t->bp,&t->pageHandler);			    
fact+=(var->slot * tempVar.recordSize);			   
*fact = '$';								    
fact=fact+1;
int tempsize=tempVar.recordSize;	                                		           
memmove(fact,record->data+1,tempsize);
unpinPage(&t->bp, &t->pageHandler);			    
pinPage(&t->bp, &t->pageHandler, 1);					    			    	    
return RC_OK;								    
}


RC deleteRecord (RM_TableData *rel, RID id)			            
{
openPageFile(t->name, &smfileHandler);			   
fact = t->pageHandler.data; 
int holder1=t->noOfTuples;
int holder2=getRecordSize(rel->schema);					   
tempVar.value = holder1; 						   
tempVar.recordSize = holder2;  		
int tempadd=(id.slot * tempVar.recordSize);    
tempVar.value +=tempadd; 	
dirtyandunpin(t); 			    		    
return RC_OK;	
}


int dirtyandunpin(tableDetails *t)
{
	markDirty(&t->bp, &t->pageHandler); 		  
	unpinPage(&t->bp, &t->pageHandler); 
	return -1;
}


RC updateRecord (RM_TableData *rel, Record *record)							
{
						    
pinPage(&t->bp, &t->pageHandler, record->id.page); 	    
fact = t->pageHandler.data;
int holder1=getRecordSize(rel->schema);					    
tempVar.recordSize = holder1;
var = &record->id;	
int tempadd=(var->slot * tempVar.recordSize);	   	
fact = fact + tempadd; 			    
fact=fact+1;						            
memmove(fact,record->data + 1,holder1); 
dirtyandunpin(t); 			    	   
return RC_OK;	
						    
}



RC getRecord (RM_TableData *rel, RID id, Record *record)			
{
pinPage(&t->bp, &t->pageHandler, id.page); 	            
tempVar.recordSize = getRecordSize(rel->schema);
pointer = t->pageHandler.data;	
int tempadd=((id.slot)*tempVar.recordSize);			
pointer = pointer+tempadd;	
fact = record->data;							   
fact=fact+1;				
int holder1=tempVar.recordSize - 1;			   
memmove(fact,pointer + 1,tempVar.recordSize - 1);			    
record->id = id; 			                        	    
unpinPage(&t->bp, &t->pageHandler); 		   
return RC_OK;
							
}

RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){
	char *name;
	openTable(rel,name);
	ScanInfo *scanner;
	scanner = (ScanInfo*) malloc(sizeof(ScanInfo));
	tableDetails *td;	
	td = rel->mgmtData; 						    
	td->noOfTuples = 25;	
	scanner->currentRow.slot= 0;							     
	scanner->currentRow.page= 1;			     
	scanner->condition = cond;                                                    
	scan->mgmtData = scanner;
	scan->rel= rel;	
	return RC_OK;	
}


RC closeScan (RM_ScanHandle *scan)
{

tableDetails *td;
td = (tableDetails*)scan->rel->mgmtData;
ScanInfo *scanner;
scanner = (ScanInfo*) scan->mgmtData;
unpinPage(&td->bp, &scanner->pageHandler);
scanner->currentRow.page= 1;                                                             
scanner->currentRow.slot= 0;                                                            
scanner->numTuplesScanned = 0;                                                       
return RC_OK;	                                                                          
}

RC next (RM_ScanHandle *scan, Record *record)								
{
ScanInfo *scanner;
scanner = (ScanInfo*)scan->mgmtData;
tableDetails *td;
td = (tableDetails*)scan->rel->mgmtData;
Value *va;                                                                     
va = (Value *) malloc(sizeof(Value));

tempVar.slots = ceil(4096/tempVar.recordSize);
tempVar.recordSize = getRecordSize(scan->rel->schema);	
	       
scanner->numTuplesScanned=0;					
while(scanner->numTuplesScanned <= td->noOfTuples)			
	{                                                                           
		scanner->currentRow.slot = scanner->currentRow.slot + 1;				
		pinPage(&td->bp, &scanner->pageHandler, scanner->currentRow.page);
		newFact = scanner->pageHandler.data;				
		newFact = newFact + (scanner->currentRow.slot * tempVar.recordSize);
		record->id.page = scanner->currentRow.page; 				
		record->id.slot = scanner->currentRow.slot;				
		fact = record->data;					
		fact = fact + 1;					
		memmove(fact,newFact+1,tempVar.recordSize-1); 		
		evalExpr(record,(scan->rel)->schema, scanner->condition, &va);
		scanner->numTuplesScanned++;                                     
		if((va->v.floatV == TRUE)||(va->v.boolV == TRUE))		
		{
			unpinPage(&td->bp, &scanner->pageHandler);		
			return RC_OK;						
		}
	}
unpinPage(&td->bp, &scanner->pageHandler);
scanner->numTuplesScanned = 0; 		
scanner->currentRow.page = 1;				         
scanner->currentRow.slot = 0;
				
return RC_RM_NO_MORE_TUPLES;				
}


int getRecordSize (Schema *schema)
{
	if (schema == NULL) {
		THROW(RC_INVALID_HANDLE, "Schema handle is invalid");
	}

tempVar.value = 0;
tempVar.fsetAttributes = 0;                                           
                                                               
while(tempVar.value<schema->numAttr)                                             
	{                                                                           
		if (schema->dataTypes[tempVar.value] == DT_STRING)                             
			{                                          
				tempVar.fsetAttributes = tempVar.fsetAttributes + schema->typeLength[tempVar.value];                           
			} 
		else if(schema->dataTypes[tempVar.value] == DT_INT)                     
			{
				tempVar.fsetAttributes = tempVar.fsetAttributes + 4; 
			}
		tempVar.value++;
	}                                                                           
tempVar.fsetAttributes = tempVar.fsetAttributes++;                              
return tempVar.fsetAttributes;                                                
}

Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{ 
Schema *schemaMemory;                                                         
schemaMemory= (Schema*) malloc(sizeof(Schema));                               
schemaMemory->numAttr= tempVar.attributes;                                        
schemaMemory->attrNames= (char**) malloc(sizeof(char*)*3);                    
schemaMemory->dataTypes= (DataType*) malloc(sizeof(int)*3);                   
schemaMemory->typeLength= (int*) malloc(sizeof(int)*3);  

                                                         // Memory Allocation
schemaMemory->numAttr = numAttr;                                            // Initializing the No. of Attributes
schemaMemory->attrNames = attrNames;                                        // Initializing the Attribute Names
schemaMemory->dataTypes =dataTypes;                                         // Initializing the Attribute's data types
schemaMemory->typeLength = typeLength;                                      // Initializing the Attribute's type length
schemaMemory->keySize = keySize;                                            // Initializing the Key size
schemaMemory->keyAttrs = keys;                                              // Initializing the key attribute
return schemaMemory;                                                        // Returns the value of Schema
}

RC freeSchema (Schema *schema)
{
free(schema);                                                              
return RC_OK;
}

/* DEALING WITH RECORDS AND ATTRIBUTE VALUES */

RC createRecord(Record **record, Schema *schema)                       
{                
tempVar.attributes = schema->numAttr;	                             
int *length = schema->typeLength;					       
tempVar.value=0;							       
DataType *dt = schema->dataTypes;				       
for( tempVar.value=0;tempVar.value < sizeof(int);tempVar.value++) 					      
{
tempVar.pos = *(dt + tempVar.value);		                       
if (tempVar.pos == DT_INT) 				       
{
tempVar.recordSize = tempVar.recordSize + sizeof(int);		       
} 
else if (tempVar.pos == DT_FLOAT)			       
{
tempVar.recordSize = tempVar.recordSize + sizeof(float);		      
}
else if (tempVar.pos == DT_BOOL) 			      
{
tempVar.recordSize = tempVar.recordSize + 1;			      
} 
else if (tempVar.pos == DT_STRING)
{ 
tempVar.recordSize = tempVar.recordSize + (*(length + tempVar.value));	      
}						               
}

for( tempVar.value=0;tempVar.value < sizeof(int) + sizeof(char);tempVar.value++)
{			  					
pointer = (char *)malloc(sizeof(int) + sizeof(char));		      
pointer[tempVar.value]='\0';						       
*record = (Record *)malloc(sizeof(int));			       
record[0]->data=pointer;					       
}						       
return RC_OK;								
}

RC freeRecord (Record *record)
{
free(record);                                      			                    
return RC_OK;								
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value) 
{
tempVar.fsetAttributes = 0; 				     
attributes(schema, attrNum, &tempVar.fsetAttributes);  
Value *v;                                                                    
v = (Value *) malloc(sizeof(Value));
					     
pointer = record->data;					     
pointer = pointer + tempVar.fsetAttributes;		     
if(attrNum == 1)					     
{
schema->dataTypes[attrNum] = 1;				     
}
switch (schema->dataTypes[attrNum])
{
      case DT_STRING:
	  {
	v->dt = DT_STRING;					    
	tempVar.strLength = schema->typeLength[attrNum];	     
	v->v.stringV = (char *) malloc(sizeof(char));		     
	strncpy(v->v.stringV, pointer, tempVar.strLength);	
	v->v.stringV[tempVar.strLength] = '\0';	
	break;
	  }
      case DT_BOOL:
	  {
	v->dt = DT_BOOL;					     
	bool boolValue;					     
	memmove(&boolValue,pointer,1);			     
	v->v.boolV = boolValue;	
	break;
	  }
      case DT_INT:
	  {
	memmove(&tempVar.value,pointer, 4);				    
	v->v.intV = tempVar.value;					     
	v->dt = DT_INT;
	break;
	  }
      case DT_FLOAT:
	  {
	v->dt = DT_FLOAT;					     
	float floatValue;					     
	memmove(&floatValue,pointer,4);			     
	v->v.floatV = floatValue;
	break;
	  }
     default:
	 {
      	return RC_INVALID_DATATYPE;
		break;
      }
}	  
*value = v;						     
return RC_OK;						     
}

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
tempVar.fsetAttributes = 0;			              
attributes(schema, attrNum, &tempVar.fsetAttributes);   
pointer = record->data;					      
pointer = pointer + tempVar.fsetAttributes;
switch (schema->dataTypes[attrNum])
{
      case DT_STRING:
	  {
	tempVar.recordSize = schema->typeLength[attrNum]; 	      
	newFact = (char *) malloc(sizeof(char)+sizeof(int));	      
	strncpy(newFact, value->v.stringV, sizeof(char)+sizeof(int)); 
	newFact[tempVar.recordSize] = '\0';			      
	strncpy(pointer, newFact, sizeof(char)+sizeof(int)); 
	break;
	  }
      case DT_BOOL:
	  {
	bool boolValue;					      
	boolValue = value->v.boolV;				      
	boolValue = boolValue++;
	break;
	  }
      case DT_INT:
	  {
	*(int *)pointer = value->v.intV;
	break;
	}
      case DT_FLOAT:
	 {
	float floatValue;					      
	floatValue = value->v.floatV;			      
	floatValue = floatValue + 4;
	break;
	}
     default:
	 {
      	return RC_INVALID_DATATYPE;
	break;
      }	
}	  
return RC_OK;						      
}

int attributes(Schema *schema, int attrNum, int *final)
{
  
  tempVar.pos = 0;
  tempVar.fsetAttributes=1;
   for(tempVar.pos = 0; tempVar.pos < attrNum; tempVar.pos++)
    switch (schema->dataTypes[tempVar.pos])
      {
      case DT_STRING:
	tempVar.fsetAttributes = tempVar.fsetAttributes + schema->typeLength[tempVar.pos]; 
	break;
      case DT_BOOL:
	tempVar.fsetAttributes = tempVar.fsetAttributes + sizeof(bool); 
	break;
      case DT_INT:
	tempVar.fsetAttributes = tempVar.fsetAttributes + sizeof(int); 
	break;
      case DT_FLOAT:
	tempVar.fsetAttributes = tempVar.fsetAttributes + sizeof(float); 
	break;
      
      }
  
  *final = tempVar.fsetAttributes;
  return RC_OK;
}



int AddEmptySpace(char *value, int sizeOfRecord)
{
tempVar.value = 0;
tempVar.slots = ceil(4096/sizeOfRecord);    //PAGE_SIZE = 4096 bytes
while(tempVar.slots>tempVar.value)
{
if (value[tempVar.value * sizeOfRecord] != '$')
	return tempVar.value;
tempVar.value=tempVar.value+1;
}
return -1;
}


int AddPageInfo(RID *var,tableDetails *t)//Assigning page information and Record info
{
var->page = t->slotsEmpty;					    
pinPage(&t->bp, &t->pageHandler, var->page);   
fact = t->pageHandler.data;					    
return -1;
}


int Checkslot(RID *var,tableDetails *t)
{
	while(!((var->slot)>=0))
	{                                                                           
		var->page=var->page+1;								    
		pinPage(&t->bp, &t->pageHandler, var->page);	    
		fact = t->pageHandler.data;					   
		var->slot = AddEmptySpace(fact, tempVar.recordSize);
			    
	}
return -1;
}


