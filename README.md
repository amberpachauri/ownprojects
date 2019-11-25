Tutorial
===========
A step guide to create binary or executable .

Presently, still in-process to create Makefile for Cross-Platform compilation.

Updated changes to code: 
----Mon 16 Jul 15:41:56 BST 2018----
@ changes to add comments to code and module description
@ changes to prevent memory leak. 
@ changes to add url response.




Modules 
===============

This project has four important module:-

[1] user program, main module[C file], to interact with Sam's library - 
      	|
	____user.c 

[2] interface library module [C file], to interact with user-program & CURL library - 
	|
	____samknowsDTMAPI.c

[3] library header file to support enums, structure and definitions for library interface- 
	|
	_____samknowsDTMAPI.h

[4] header file to support definitions, and struture for user program and library 
	|
	___common.h


Compilation Process
=====================

>> [1] create folder to store all headers file[*.h]  - ./libINC

>> [2] create folder to store library [*.so] - ./lib

>> [3] execute commands on top-level :

	$> gcc -c -g -w -fpic -I./libINC  samknowsDTMAPI.c
            
	$> gcc -shared -fpic -o libsmknwsDTMAPI.so samknowsDTMAPI.o   

	$> cp libsmknwsDTMAPI.so ./lib/

**ps: confirm library module, is created.

	$> gcc -w -o userapp user.c -lsmknwsDTMAPI -lcurl -Llib
**ps: confirm library module, is created.


Execute Binary
=================
To execute user application :

	$> ./userapp -ulinux.org -s5 -h"Accept: application/xml" -h"Content-Type: application/xml"


Output/ Result
================


SKTEST; <IP server>;<response code>;<median-lookup( 0.020254s)>;<Median-connect( 0.056010s)>;<Median-transfer( 0.162594s)>;<Median-total( 0.227812s)>

(ps: IP, and response code in 2nd commit/ changes few seconds away...!)


Enjoy!
Happy Programming with 'C'.

Send any queries, comments, or any feature changes to - amberpachauri@rediffmail.com

<html><i>ambersone</i></html>
