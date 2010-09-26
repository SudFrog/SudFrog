
SudFROG sudparis Face RecOgnition with Gabor

*** README OF COMPLIE AND INSTALLATION ***
(Last update - 2609/2010 by Dianle ZHOU) 
Note: commands to type are initial with “>” 
******************************************

1-- Downloading:
	a) The sud-forg face recognition system is currently available on the server “jaguar.it-sudparis.eu” at the follow address.
		> /data/preprocessing/frgc/frog-0.1.tar.gz
	b) Download the archive frog-0.1.tar.gz in your home directory /home/xxxx for example. And then decompress it as follows:
 		> tar zxvf frog-0.1.tar.gz
 	c) Download  from the SVN  in your home directory
    > svn co http://svnext.it-sudparis.eu/svnview2-eph/ref_syst/sudfrog
2-- Compilation:
     To compile the source code, make sure the fftw2, libjpeg and libthread has been installed. And if you do it on Jaguar, those packages have already been installed. 
	a) OpenCV is supposed to be installed also.
	b) Enter in the source directory:
		> cd frog/
	c) Configure the programs: (replace /home/xxxx/ with your own folder)	
		> autoreconf -vi
		> ./configure --prefix=/home/xxxx/bin/
	d) Compile the source code:
		> make&&make install
	e) Set environment variable:
		> export PATH=/home/xxxx/bin/bin:$PATH
2-- Description:
	
At the end of these steps, nine binaries are created:

[xxxx@jaguar frog]$ ls -al /home/xxxx/bin/bin
total 32620
drwxr-xr-x 2 xxxx eph    4096 Dec 23 13:27 .
drwxr-xr-x 3 xxxx eph    4096 Dec 23 13:25 ..
-rwxr-xr-x 1 xxxx eph 3583869 Dec 23 13:27 CompareFaces
-rwxr-xr-x 1 xxxx eph 3609373 Dec 23 13:27 CompareList
-rwxr-xr-x 1 xxxx eph 2866291 Dec 23 13:27 CompareTemplate
-rwxr-xr-x 1 xxxx eph 3925389 Dec 23 13:27 frgc_run
-rwxr-xr-x 1 xxxx eph 1751705 Dec 23 13:27 GetPerformance
-rwxr-xr-x 1 xxxx eph 4086838 Dec 23 13:27 mbgc_run
-rwxr-xr-x 1 xxxx eph 2394396 Dec 23 13:27 Normfaces
-rwxr-xr-x 1 xxxx eph 3764437 Dec 23 13:27 SpaceLearner
-rwxr-xr-x 1 xxxx eph 3523828 Dec 23 13:27 TemplateFace
-rwxr-xr-x 1 xxxx eph 3827083 Dec 23 13:27 TemplateList

For the details of description of each executable file : explanation, input and output information et, please find in the documentation.pdf from page 2 to page 12, which could be found at: latex/documentation.pdf.

