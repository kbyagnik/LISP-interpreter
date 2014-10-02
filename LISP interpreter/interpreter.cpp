#include <cstring>
#include<stdio.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include<stdlib.h>
using namespace std;

class genlistnode {		//Class of a Generalized Linked List
	public:
	bool flag;	
	string s;
	genlistnode *down, *next;	
};

class value {		//class of the floating value of a list-node
	public:
	int tag;
	float val;
	genlistnode* link;
};

class env {		//class of environment of variables in lis
	public:
	value* table[26];
	env* parent;

	//constructor
	env() {
		for(int i=0;i<26;i++)
		table[i]=NULL;
		parent=NULL;
	}

};

value evallist(genlistnode *p,env &e);			//definition of evallist function

void add(char x,value val_new,env &e){			//adding a new value val_new of variable x in env e
	value *p=new value;
	
	p->tag=val_new.tag;
	p->link=val_new.link;
	p->val=val_new.val;

	e.table[x-'a']=p;
}

value find(char x,env &e)				//finding the value of variable x in env e (or previous)
{
	if (e.table[x-'a']!=NULL)
		return *(e.table[x-'a']);	

	else if (e.parent!=NULL)
		return find(x,*(e.parent));
}

value evalnode(genlistnode *p,env &e){			//evaluating a genlistnode in env e 
	
	if (p->flag==1)					//if its a list
		return evallist(p->down,e);
	
	else if ( isalpha(p->s[0]) )			//if its variable
		return find(p->s[0],e);
	
	else if (!(isalpha(p->s[0])))			//it is a constant
	{
		value v;
		v.link=NULL;
		v.tag=0;
		v.val=atof(p->s.c_str());
		//return value obtained by converting the string p->s to floating point
		return v;
		}
	
};

value evallist(genlistnode *p,env &e){
		
		if ( p->s=="begin" ){			//keyword begin
			p=p->next;
			while (p->next!=NULL){
				evalnode(p,e);
				p=p->next;
			}
			
			return evalnode(p,e);
		}
		
		else if (p->s=="define"||p->s=="set"){	//keywords define and set
			value v;
			p=p->next;
			v=evalnode (p->next,e);
			add (p->s[0],v,e);
			
			
			
			return v;
		}
		
		else if (p->s=="if"){			//keyword if
			p=p->next;
			value v=evalnode(p,e);
			if (v.val!=0)
				return evalnode(p->next,e);
			else 
				return evalnode(p->next->next,e);
		}
		
		else if (p->s=="while"){		//keyword while
			p=p->next;
			bool b=0;
			value v=evalnode(p,e),v1;
			while (v.val!=0)
			{
				b=1;
				v1=evalnode(p->next,e);
				v=evalnode(p,e);
				
			}
			if (b==0)
				return v;
			return v1;
		}
		
		else if (p->s=="lambda"){		//keyword lambda
			value new_val;
			new_val.tag=1;
			new_val.link=p;
			return new_val;
		}
		
							/*p->s is an operator*/
		else if (p->s=="<="||p->s==">="||p->s==">"||p->s=="<"||p->s=="=="||p->s=="!="||p->s=="%"||p->s=="+"||p->s=="-"||p->s=="*"||p->s=="/")
		{
			value v1,v2,v3;
			v1=evalnode(p->next,e);
			v2=evalnode(p->next->next,e);
			v3.tag=0;
			v3.link=NULL;
			
				if (p->s=="+"){
				v3.val=v1.val + v2.val ;
				return v3;
			}
			else if (p->s=="-"){
				v3.val=v1.val - v2.val ;
				return v3;
			}
			else if (p->s=="*"){
				v3.val=v1.val * v2.val ;
				return v3;
			}
			
			else if (p->s=="/"){
				v3.val=v1.val / v2.val ;
				return v3;
			}
			else if (p->s=="%"){
				v3.val=(int)v1.val % (int)v2.val ;
				return v3;
			}
			
			else if (p->s==">"){
				if (v1.val > v2.val)
				v3.val=1;
				else v3.val=0;
				return v3;
			}
			else if (p->s=="<"){
				if (v1.val < v2.val)
				v3.val=1;
				else v3.val=0;
				return v3;
			}
			else if (p->s==">="){
				if (v1.val >= v2.val)
				v3.val=1;
				else v3.val=0;
				return v3;
			}
			else if (p->s=="<="){
				if (v1.val <= v2.val)
				v3.val=1;
				else v3.val=0;
				return v3;
			}
			else if (p->s=="=="){
				if (v1.val == v2.val)
				v3.val=1;
				else v3.val=0;
				return v3;
			}
			else if (p->s=="!="){
				if (v1.val != v2.val)
				v3.val=1;
				else v3.val=0;
				return v3;
			}
			
		}
		
		else if ((isalpha(p->s[0])) || ( p->flag==1))	//p->s is a variable
		{
			
			genlistnode *f;
		if (p->flag==1)					//if its a list
			f=p->down;
		else
			{
				value v=evalnode(p,e);				
				f=v.link;
			}
		
	

		genlistnode *arg_list = f->next->down;
		genlistnode *body = f->next->next;
		
		env e1;
		e1.parent=&e;
		
		while(arg_list!=NULL){				//evaluating the body of the loop till condition satisfies
			add(arg_list->s[0],evalnode(p->next,e),e1);
			p=p->next;
			arg_list=arg_list->next;
		}
		
		return evalnode(body,e1); 
			
		}

		else return evalnode(p,e);
		
};


void printlist (genlistnode*);					//function definition

int c=0,chk=0;							//global variables for checking balanced parenthesis 

genlistnode *makelist()							//Making a new entry in genlist
{
	genlistnode *head=new genlistnode;
	genlistnode *p=new genlistnode,*l=new genlistnode;
	string t;
	head=NULL;
	while(c!=0 || chk==0)					//checking for balanced parenthesis
{	

        ++chk;
	cin>>t; 	                           																																																																																		
	if (t=="(")						//if opening brace
	{
		c++;
		l=new genlistnode;
		l->next=NULL;
		l->flag=1;
		l->down=makelist();
	}

	else if (t==")"){					//if ending brace
	c--;
	return head;
	}	
	
	else							//if it is other than delimiters
	{	
		l=new genlistnode;
		l->flag=0;
		l->s=t;
		l->down=NULL;
		l->next=NULL;
	}
		if (head==NULL )				//defining the head of the list
		{
			head=l;
			p=head;
		}
		else
		{
			p->next=l;
			p=p->next;
		}
}
	return head;

}


int main() {
	
	int i,j,k;
	value v;
	env e;
	genlistnode *p=new genlistnode;				//declaring the genlistnode
	string t;

	
	p=makelist();
	//printlist(p->down);
	
	v=evallist(p->down,e);
	cout<< v.val << endl;
	
	return 0;
}

void printlist(genlistnode *p)					//function to print the generalized list for verification
{
	if(p==NULL) return;
	cout<<p->s<<" ";
	printlist(p->down);
	printlist(p->next);
}
