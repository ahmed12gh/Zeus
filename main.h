#include "utils.h"
#include <ctype.h>

#define formulaLen 50
#define VarName_len 1024
#define VARABLEs_LIMIT 1024
#define TextLimit 1024

typedef struct {
    char name [VarName_len] ; 
    double value ; 
    } varable ; 




varable search_List(varable * list , char * name , int size ){
	printf("search_list(): name to find %s \n" , name) ;
    for(int i = 0 ; i < VARABLEs_LIMIT ; i++ ){
        if ( is_arrayDifferSize(list[i].name , name , size) ){
            printf (" Search list ; found it \n "); 
            return list[i] ;  
        }

    }
    varable var = {"-1" , -1 }  ; 
    return var ; 
}

varable* search_ListPtr(varable * list , char * name , int size ){
	printf("search_list(): name to find %s \n" , name) ;
    for(int i = 0 ; i < VARABLEs_LIMIT ; i++ ){
        if ( is_arrayDifferSize(list[i].name , name , size) ){
            printf (" Search list ; found it \n "); 
            return &list[i] ;  
        }

    }
    static varable var = {"-1" , -1 }  ; 
    return &var ; 
}

void printVar(varable var ){
	printf("varable %s = %d\n" , var.name , var.value) ;
}

void setConfig(char * fn  ) ; 


// Number = {{numlen} , lastfilledindex , empty  , opr , num }
// array = { {lenarray} , lastfilledindex }

// BUGS : 

char opraters[] = {'+', '-', '/', '*', '^', '~' , '(', ')'};

void removeReturnChar(char *arr)
{
	for (int i = 0; i < strlen(arr); i++)
	{
		if (arr[i] == '\n')
		{
			arr[i] = '\0';
		}
	}
}
	
void appendToVarList(varable* list , int* lastIndex , varable var ){
	if(*lastIndex < VarName_len){
	list[*lastIndex] = var ; 
	*lastIndex +=1 ; 
	}else{
		printf("appendtoVarList() : something is wrong lastindex is bigger than size ") ; 
	}

}
	
NUMBER do_formulaArr(Array arr) ; 

Array reWriteArray(Array BaseArr, NUMBER value, int Ind [formulaLen] , int size)
{
	Array newArr = {{}, 0};
	fillnullArray(&newArr) ; 
	bool Done = false;
	for (int i = 0; i < formulaLen; i++)
	{	
		if (intInIntArray(i , Ind , size))
		{
			// do nothing to remove 
			printf("remove index %d in base array. \n  " , Ind[i]); 
			if (Done == false)
			{ 	// add the value for once 
				printf("adding value to arr"); 
				PrintNum(value); 	
				append2Array(&newArr, value);
				Done = true;
			}
		}
		else
		{
			// adding all other things 
			if (is_equalNum(BaseArr.arr[i] , emptyNumber)) break ; // reach the end  
			append2Array(&newArr, BaseArr.arr[i]);
		}
	}
	return newArr;
}

Array power_root(Array arr)
{
	Array newFormula = {{}, 0};
	newFormula = arr ; 
	char check_list[] = "^~";
	for (int i = 0; i < formulaLen; i++)
	{
		char *c = arr.arr[i].arr;
		
		if (charInStr(check_list, c[0]) && arr.arr[i].lastfilledIndex <= 1 ) 
		{
			if (i == 0 || i == formulaLen )
			{
				continue;
			}
			printf("found %c in %d \n" , c[0] , i ) ; 
			NUMBER value = float2number(calc(arr.arr[i - 1], arr.arr[i + 1], arr.arr[i]));
			int removedIndexs [] = { i -1 , i , i + 1 } ; 
			newFormula = reWriteArray(arr, value, removedIndexs , 3);
			break;
		}
	}
	if ((NumInArray(RootNumber , newFormula) || NumInArray(PowerNumber, newFormula)) && sizeOfArray(newFormula) > 3 )
	{
		newFormula = power_root(newFormula);
	}
	return newFormula;

}

Array mult_div(Array arr)
{
	Array newFormula = {{}, 0};
	newFormula = arr ; 
	char  check_list [] = "*/";
	for (int i = 0; i < formulaLen; i++)
	{
		char *c = arr.arr[i].arr;
		if (charInStr(check_list, c[0]) && arr.arr[i].lastfilledIndex <= 1 ) 
		{
			if (i == 0 || i == formulaLen - 1)
			{
				continue;
			}
			printf("%c %c %c in %d \n " ,arr.arr[i-1].arr[0] ,  c[0] , arr.arr[i+1].arr[0] , i ) ; 
			NUMBER value = float2number(calc(arr.arr[i - 1], arr.arr[i + 1], arr.arr[i]));
			int removedIndexs [] = { i -1 , i , i + 1 } ; 
			newFormula = reWriteArray(arr, value, removedIndexs, 3);
			break;
		}
	}
	if ((NumInArray(MultNumber, newFormula) || NumInArray(divNumber, newFormula) )  && sizeOfArray(newFormula) > 3)
	{
		newFormula = mult_div(newFormula);
	}
	return newFormula;
}

Array add_min(Array arr)
{
	Array newFormula = {{}, 0};
	newFormula = arr ; 
	char check_list[] = "+-";
	for (int i = 0; i < formulaLen; i++)
	{
		char *c = arr.arr[i].arr;
		if (charInStr(check_list, c[0]) && arr.arr[i].lastfilledIndex <= 1 ) 
		{
			if (i == 0 || i == formulaLen - 1)
			{
				continue;
			}
			printf("found %c in %d \n" , c[0] , i ) ; 
			NUMBER value = float2number(calc(arr.arr[i - 1], arr.arr[i + 1], arr.arr[i]));
			int removedIndexs [] = { i -1 , i , i + 1 } ; 
			newFormula = reWriteArray(arr, value, removedIndexs , 3);

			break;
		}
	}
	if ((NumInArray(MinNumber , newFormula) || NumInArray(PlusNumber , newFormula)) && sizeOfArray(newFormula) > 3 )
	{
		printf("addMin() : redoing \n") ; 
		newFormula = add_min(newFormula);
	}
	return newFormula;
}

Array PrthsiscesFormula(Array arr){
	int StartPrths = 0 ; int EndPrths = 0 ;
	int StartPrthIndex = -1 ; int  EndPrthIndex  = -1 ; 
	
	int IndexsToRemove [formulaLen] ; 
	Array TodoArr = {{} , 0} ; fillnullArray(&TodoArr) ;  
	Array NewArr = {{} , 0} ; fillnullArray(&NewArr) ;  

	for (int i = 0; i < formulaLen; i++  )
	{

		char c = arr.arr[i].arr[0] ; 
		printf("%c , " , c) ;
		printf(" \n")  ; 
		if(c == '('){
			printf("found an '(' in %d \n " , i) ;
			if(StartPrthIndex == -1){ // first start prthisis found 
				StartPrthIndex = i ; 
			}
			StartPrths += 1  ;  // increase the number of current prthesiss
		}
		if ( c == ')'){
			printf("found an ')' in %d \n " , i );
			if(StartPrths -1 == EndPrths ){ // if need one to complete 
				EndPrthIndex = i ; 
			}
			EndPrths += 1 ; // increase the number of current prthesiss
		}
		// found a complete brakets 
		if( StartPrths == EndPrths && StartPrths != 0  ){
			printf("break in i = %d \n " , i );
			break; 
		}
	}
	
	int ArrIndex = 0 ; int size = 0  ; 
	// make the array to remove and replace 
	for ( int i = StartPrthIndex ; i <=   EndPrthIndex ; i++)
	{
		IndexsToRemove[ArrIndex] = i ;
		ArrIndex ++ ;  
		size += 1 ; 
	}

	// make the array to solve 
	int todoarrIndex = 0; 
	NUMBER startPrinths = {{"("} , 1};
	NUMBER EndPrinths = {{")"} , 1};

	for (int i = StartPrthIndex +1; i < EndPrthIndex ; i++)
	{
		if(EndPrthIndex-StartPrthIndex == 1 ) break ; // if empty 
		else TodoArr.arr[todoarrIndex ++] = arr.arr[i] ; 
		
	}
	

	if( StartPrths == EndPrths && EndPrths != 0 ){
		NUMBER result = do_formulaArr(TodoArr) ; 
		NewArr = reWriteArray(arr , result , IndexsToRemove , size   ); 

		printf("prthisicessformula(): array after proccesing \n ") ;
		PrintArray(NewArr);
		
		printf("prthisicessformula(): indexs arr after proccesing \n ") ;
		printIntArr(IndexsToRemove) ; 
		 
		if (charInArray('(', NewArr))
		{	
			NewArr = PrthsiscesFormula(NewArr) ; 
		}

	}

	return NewArr ; 

}

// check if the NUMBER sturct is in oprater arr  
bool NumberIsOprater(NUMBER num){
	if(is_equalNum (num ,  PlusNumber)) return true ; // +
	if( is_equalNum(num , MinNumber ))  return true ; // -
	if(is_equalNum(num ,  MultNumber)) return true ; // * 
	if(is_equalNum (num , divNumber ))  return true ; // / 
	if(is_equalNum (num , RootNumber)) return true ; // r
	if(is_equalNum(num , PowerNumber)) return true ; // ^  
	else return false ; 
}


NUMBER do_formulaArr(Array arr)
{

	Array NewFormula = {{}, 0};
	NewFormula = arr ; 
	printf("input Array : ");
	PrintArray(NewFormula) ; 

	if (charInArray('(', NewFormula))
	{	
		NewFormula =  PrthsiscesFormula(NewFormula);  
		printf("after doing the () : ") ; 
		PrintArray(NewFormula) ; 
	}

	if (charInArray('^', NewFormula) || charInArray('~', NewFormula))
	{
		NewFormula = power_root(NewFormula);
		printf("after ^ & r : "); 
		PrintArray(NewFormula);
	}

	if (charInArray('/', NewFormula) || charInArray('*', NewFormula))
	{
		NewFormula = mult_div(NewFormula);
		printf("after * & / : "); 
		PrintArray(NewFormula);
	}
 
	if (charInArray('+', NewFormula) || charInArray('-', NewFormula))
	{
		NewFormula = add_min(NewFormula);
		printf("after + & - and  output : "); 
	}

	printf("result is : "); 
	PrintArray(NewFormula);
	if ( NumberIsOprater(NewFormula.arr[0])) {
		NUMBER zero = {{"0"}, 1} ; 
		return zero ; 
	}
	return NewFormula.arr[0] ; 
}

varable check_Varables(char * text , int len ){
	printf ( "len is %d \n" , len ); 
	varable var ; 
	int index ;  // last element to remove from array 
	fillNullCharArray(var.name) ;  

	for (int i = 0 ; i < len ; i++ ){
		char c = text[i] ; 
		if(isalpha(c)){
			append2charArray(var.name , VarName_len  , c); 
		}
		if(c == '=') {
			index = i + 1  ; 
			break ; 
		}
	}

	printf ( "Check_varables : text shifting is %s" , text );  
	// shifting the array to left by index 
	memmove( text , & text[index] , (VarName_len -index ) * sizeof(char)) ; 
	text[ index * -1  ] = '\0' ; 

	printf ( "Check_varables() : text shifting is %s" , text );  
	printf ( "check_varables() :  name of the varable is %s \n " , var.name ); 
	printVar(var) ; 
	return var ;

}


bool allVarsReplaced(char * text , int len ){
	// return false postive 
	printf("ALlVarsReplaced() : text = %s \n " , text ); 
	for(int i =0 ; i < len  ; i ++){
		if(isalpha(text[i])) {
			printf("allvarsReplaced() : false \n") ;
			return false ; 
		}
		
	}
	printf("allvarsReplaced() : true \n") ;
	return true ; 
}

int replace_Varables_by_values(varable* list , int lastfilled , char * text , int len , int start , char* temp , int tempIndex  ){ 
	printf("replace varables() \n "); 
	bool found = false ; 
	char name [1] ; fillNullCharArray(name) ; 
	int index = -1 ; 

	// search text for varables 
	for(int i = start ; i< len ; i++ ){
		char c = text[i]; 
		
		//found a varables 
		if(isalpha(c) && c != '~'){
			name[0] = text[i] ;
			index = i ;  
			found=true ; 
			break ; 
		}
	}
	if (!found) // not found any varable ; 
	{
		return 0 ; // get out 
	}
	
	printf("replace_varables() :found varable %s in %d \n" , name , index) ; 

	// replace varables by there values 
	printf("replace_varables() :searching for varable %s \n" ,  name)  ; 
	varable var = search_List(list , name , 1 ); 
	if(is_str("-1" , var.name )){
		return 0; 
	}
	printf("Replace_varables() :found varables %s = %f\n" , var.name , var.value) ; 
	
	// convert float to text
	char value[LENNUMBER] ; 
	int valueIndex = 0 ; 
	fillNullCharArray(value) ; 
	sprintf(value , "%f" , var.value) ; 

	int lenOfValue = strlen(value) ; 

	// creating a temp text
	char temptext [len];
	fillNullCharArray(temptext) ; 
	if(temp != NULL) strcpy(temptext , temp ) ;  // if argument is supplet
	else  strcpy(temptext , text)  ;

	
	// add a value 
	int originalIndex = 0 ;   
	bool done = false ; 
	for(int i = index ; i < len + lenOfValue ; i++){
		
		if(value[valueIndex]!='\0'){
		text[i] = value[valueIndex++]; 
		}
		else{ 
		originalIndex = i  ; 
		break ; 
		} 
	}

	// add the text back 
	int tempI ; 
	if ( start == 0 ) tempI = index + 1;  
	else tempI = tempIndex ;  
	
	for (int i = originalIndex ; i < len + lenOfValue   ; i++ ){
		// add a * between two varables
		text[i] = temptext[tempI++] ; 

		if(isalpha(text[i]) && text[i] != '~') break ; // found a var 
			 
		printf("replace Varables() : temptext[%d] = %c \n" , tempI , temptext[tempI]) ; 
	}

	if (allVarsReplaced(text , len+lenOfValue ) == false ){
		printf("redoing \n ") ;
		replace_Varables_by_values(list , lastfilled , text , len+lenOfValue , originalIndex , temptext , tempI ) ; 
	}
	else { 
		return 0 ; 
	}



}

void MultplieCloseVar(char* text) {
	// char *tempText = (char*) malloc(sizeof(char) * (strlen(text))) ; 
	// fillNullCharArray(tempText) ; 
	char temp [strlen(text)] ;
	ZeroMemory(temp , sizeof(temp)); 
	int tempIndex = 0 ;
	int i = 0 ; 
	for(; text[i] ; i++){
		if(text[i] != '~'){
			if(isalpha(text[i]) && isalpha(text[i+1]) && text[i+1] != '~'){
				temp[tempIndex++] = text[i] ; 
				temp[tempIndex] = '*' ;
			}
			else if(isdigit(text[i]) && isalpha(text[i+1]) && text[i+1] != '~'){
				temp[tempIndex++] = text[i] ; 
				temp[tempIndex] = '*' ;
			}
			else if(isdigit(text[i+1]) && isalpha(text[i])){
				temp[tempIndex++] = text[i] ; 
				temp[tempIndex] = '*' ;
			}
			else temp[tempIndex] = text[i] ;  
			
			tempIndex ++ ; 
		}else{
			temp[tempIndex++] = '~' ; 
		}
	}
	// ZeroMemory(text , (sizeof(char) * (strlen(text) ))) ; 
	// fillNullCharArray(text) ; 
	sprintf(text , temp) ; 
	printf("MultplieVars() : %s\n" , text) ; 
	// free(tempText) ; 
}


Array decode_formula(char *input)
{
	NUMBER EMPTYn = {{}, 0, true};
	NUMBER formula[formulaLen];
	for (int i = 0; i < formulaLen; i++)
	{
		formula[i] = EMPTYn;
	}

	int index = 0;
	for (int i = 0; i < strlen(input); i++)
	{
		char c = input[i];

		if (isdigit(c))
		{	
			NUMBER num = {{}, 0, false}; 
			strcpy(num.arr, formula[index].arr); // value before appending 
			num.lastfilledIndex = LastFilledIndexNum(num); // calc the last index to append 
			append2Num(&num, c); // appending 
			formula[index] = num; // seting it to preves value 
		}

		else if (charInStr(opraters, c) == true)
		{
			if (formula[index].empty == false)
			{
				index++;
			}
			if( (NumberIsOprater(formula[index-1]) && c == '-' ) || (index == 0 && c == '-' )   ){
				NUMBER num = {{}, 0, false}; 
				strcpy(num.arr, formula[index].arr); // value before appending 
				num.lastfilledIndex = LastFilledIndexNum(num); // calc the last index to append 
				append2Num(&num, c); // appending 
				formula[index] = num; // seting it to preves value 
				continue ; 
			} 
			 // if the number is negtive
			NUMBER opr = {{}, 0, false};
			append2Num(&opr, c);
			formula[index] = opr;
			index++;
		}
		// dot of the number 
		else if( c == '.' ){
			NUMBER num = {{}, 0, false}; 
			strcpy(num.arr, formula[index].arr); // value before appending 
			num.lastfilledIndex = LastFilledIndexNum(num); // calc the last index to append 
			append2Num(&num, c); // appending 
			formula[index] = num; // seting it to preves value 
		}
	}

	Array arr = {{}, 0};
	fillnullArray(&arr);
	for (int i = 0; i < formulaLen; i++)
	{
		if (formula[i].empty == true)
		{
			break;
		}
		append2Array(&arr, formula[i]);
	}
	return arr;
}

void FillNullVarableList(varable*list){
	varable var = {"-1" , -1 }; 
	for(int i = 0;  i < VARABLEs_LIMIT ; i++){
		list[i] = var ; 
	}
}


bool CharToBool(const char * chr){
	char ch [6] ; 
	strcpy(ch , chr) ;
	// convert to lower case  
	for(int i = 0 ; ch[i] ; i++){
		ch[i] = tolower(ch[i]) ; 
	}

	if (is_str(ch , "") ) return false ; 
	if (is_str(ch , "true") ) return true ; 	
	if (is_str(ch , "on") ) return true ; 	
	if (is_str(ch , "false") ) return false ; 
	if (is_str(ch , "off") ) return false ; 
	else return false; 
}

void writeToFile(char * fn , const char * stuff){
	FILE* f = fopen(fn , "a+") ; 
	fprintf(f , stuff) ; 
	fclose(f)  ;
}

const char* NotNullorValue(const char* value , const char* def ){
	if(value == "" || value == 0) return def ;
	else return value ; 
}