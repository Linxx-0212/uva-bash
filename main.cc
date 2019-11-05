#include <stdlib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
using namespace std;

void parse_and_run_command(const std::string &command) {
    // TODO: Implement this.
    // Note that this is not the correct way to test for the exit command.
    // For example the command `exit` should also exit your shell.
	std::vector<std::string> tokens;
    std::istringstream s(command);
    std::string token;
    
    char *env[] = {NULL};
    while (s >> token) {
    	string newtoken = "";
    	for (int i = 0; i < token.size(); i++) {
    		if (token[i] !=' '&&token[i] != '\f'&&token[i] != '\n'&&token[i]!='\r'&&token[i]!='\t'&&token[i]!='\v') {
    			newtoken +=token[i];
    		}
    	}
    	if (newtoken != "")
    		tokens.push_back(token);
    }


    //std::cout<<tokens.size()<<std::endl;
    int i = 0;
    int size = tokens.size();
    if (size == 0) {
    	cerr<<"invalid command\n";
    }
    int count = 0;
    std::vector<std::string> newcommand;
    char buf_r[100];
    while (i < size) {
    	int status = 0;
    	int num = 0;
    	if (tokens[i] == "exit") {
    			exit(0);
    	}
    	int index =  -1;
    	string input = "";
    	string output = "";
    	int count_in = 0;
    	int count_out = 0;
    	while (tokens[ i + num] != "|") {
    		if (tokens[i + num] == "<") {
    			index = max(index, num);
    			count_in ++;
    		}
    		if (tokens[i + num] == ">") {
    			index = max(index, num);
    			count_out ++;
    		}
    		num ++;
    		if (i + num >=size)
    			break;
    	}

    	char msg[100];
    	string tmp = "";
		vector<string> pre;
    	memset(msg,'\0',sizeof(msg));
    	//if (count > 0) {
	  //   	ssize_t s = read(pipe_fd[0], msg, sizeof(msg));
			
			// for (int i = 0; i < s-1; i++) {
			// 	if (msg[i] != ' ') {
			// 		tmp += msg[i];
			// 	}
			// 	else
			// 		pre.push_back(tmp);
			// }
			// cout<<msg<<endl;
			// if (tmp.size() > 0) 
			// 	pre.push_back(tmp);
    		
		//}
		
    	count ++;
    	//cout << count;
    	if (index == num - 1) {
    		std::cerr << "invalid command\n";
    		i += num + 1;
    		continue;
    	}
    	if (num == 0 || count_in > 1 || count_out > 1) {
    		std::cerr << "invalid command\n";
    		i += num + 1;
    		continue;
    	}
    	if (num == 2 && (count_in == 1 || count_out == 1)) {
    		std::cerr << "invalid command\n";
    		i += num + 1;
    		continue;
    	}
    	if (num == 4 && count_in == 1 && count_out == 1){
    		std::cerr << "invalid command\n";
    		i += num + 1;
    		continue;
    	}
    	newcommand.clear();
    	for (int j = 0; j < num; j++) {
    		if (tokens[j+i] == "<") {
    			input = tokens[j + i + 1];
    			j++;
    			continue;

    		}
    		if (tokens[j+i] == ">") {
    			//cout << tokens[i]<<" "<<tokens[i + 1]<<endl;
    			output = tokens[j + i + 1];
    			j++;
    			continue;
    		}
    		newcommand.push_back(tokens[j + i]);
    	}
    	
    	char *args[newcommand.size() + 1];
    	char *cmd;
    	int pos = -1;
		cmd = new char[newcommand[0].size()];
		strcpy(cmd,newcommand[0].c_str());
		for (int j = 0; j < newcommand[0].size(); j++) {
			if (newcommand[0][j] == '/') 
				pos = j;
		}
		newcommand[0] = newcommand[0].substr(pos + 1,newcommand[0].size());
    	//cout << cmd<<endl;
    	//std::cout<< num <<std::endl;
    	for (int j = 0; j < newcommand.size();j++) {
    		char *arg = new char[newcommand[j].size()];
    		strcpy(arg,newcommand[j].c_str());
    		args[j] = arg;
    	}
    	//cout <<count<<":count"<<endl;
    	// if (pre.size() > 0) {
    	// 	for (int j = 0; j < pre.size(); j++) {
    	// 		char *arg = new char[pre[j].size()];
    	// 		strcpy(arg, pre[j].c_str());
    	// 		args[newcommand.size() + j] = arg;
    	// 	}
    	// }
    	//cout << tmp<<endl;
    	args[newcommand.size()] = NULL;
    	//cout<<cmd<<endl;
    	//cout<<strcmp(cmd,"/bin/true")<<endl;
    	if (strcmp(cmd,"/bin/true") == 0) {
        	
        	std::cout<<"exit status: 0\n";
        	i += num + 1;
	    	continue;
	    }
	    if (strcmp(cmd,"/bin/false") == 0) {
        	
        	std::cout<<"exit status: 1\n";
        	i += num + 1;
	    	continue;
	    }

	    int pipe_fd[2];
    	int pipe_fd2[2];
    	pipe(pipe_fd);
    	pipe(pipe_fd2);
	    
    	int pid = fork();
    	if (pid == -1) {
    		cerr << "fork error" << endl;
    	}
		if (pid == 0) {
			close(pipe_fd[0]);
			close(pipe_fd2[1]);
			
			if ( i + num < size&&count > 1) {
				dup2 (pipe_fd[1],1);
				dup2 (pipe_fd2[0],0);
				

			}
			else {
				if (count == 1 && i + num < size){
					dup2 (pipe_fd[1],1);
				}
				else
					if (i + num == size) {
						 dup2(pipe_fd2[0],0);
						
					
				}

			}
			if (count_in == 1) {
				char *name = new char[input.size()];
				strcpy(name,input.c_str());
				int inputfile = open(name,O_RDONLY);
				dup2(inputfile,0);
				close(inputfile);
			}
			if (count_out == 1) {

				char *name = new char[output.size()];
				strcpy(name,output.c_str());
				int outputfile = open(name,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
				dup2(outputfile,1);
				close(outputfile);	
			}
			close(pipe_fd[1]);
			close(pipe_fd2[0]);
        	status = execve(cmd,args,env);
        	exit(status);
        	
        	
	        //std::cout<<status<<std::endl;
	        
	    }
	    else {
	    	//close(pipe_fd2[0]);
	    	if (count > 1) {
		    	close(pipe_fd2[0]);
		 		write(pipe_fd2[1],buf_r,strlen(buf_r));
		 		close(pipe_fd2[1]);
		 	}
		    waitpid(-1,&status,0);
	    	if (count >= 1){
	    		close(pipe_fd[1]);
	    		read(pipe_fd[0],buf_r,100);
	    		close(pipe_fd[0]);
	    	}
	        if (status == 0) {
	        	cout << "exit status: 0.\n";
	        }
	        else { 
	        	cerr << "exit status: "<<status<<endl;
	        }
	        
	    }
	    	//std::cout<<status<<std::endl;
    	i += num + 1;
    }
}
    

int main(void) {
    while (true) {
        std::string command;
        std::cout << "> "; 
        std::getline(std::cin, command);
        parse_and_run_command(command);
    }
    return 0;
}
