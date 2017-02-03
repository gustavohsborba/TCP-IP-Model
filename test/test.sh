#!/bin/bash



#### FUNCTIONS


function clean_workspace 
{
	echo -e 'Deleting test files...'
	# inverts ls to delete all files except this one.
	ls | grep -v test.sh | xargs rm&>/dev/null 
}



function copy_and_build 
{
	echo -e "Copying script files and building..."
	cp ../*/* ./&>/dev/null # atualizes edited files
	# compiles what needs to be compiled...
	gcc -o physical-client physical-client.c
	gcc -o physical-server physical-server.c
}



function config_terminator 
{
	echo -e "Configuring Terminator..."
	cp ~/.config/terminator/config ../terminator-layouts/config.old
	if [ $1 == "restore" ]; then
		cp ../terminator-layouts/config.old ~/.config/terminator/config
	elif [ $1 != "server" -a $1 != "client" -a $1 != "client_udp" -a $1 != "server_udp" -a $1 != "client_browser" -a $1 != "testing" ]; then
		echo "Argumento $1 inválido! "
		exit
	else
		layout=$1
	fi
	cp ../terminator-layouts/${layout}.layout ~/.config/terminator/config
}



function run 
{
	terminator
}


function help_text {
	echo -e "USAGE: $0 [OPTIONS] [PARAMETER]\n"
	echo -e 'Without any argument: cleans, builds, prepares and run test environment.'
	echo -e 'With single argument: cleans, builds, prepares and run PARAMETER environment.'
	echo -e 'This PARAMETER shoud follow -t pattern\n'
	echo -e '\t-c\tClean Workspace: Deletes files in test folder'
	echo -e '\t-b\tBuild all. copy files into test folder and builds everything.'
	echo -e '\t-t PARAMETER\tConfigure environment. PARAMETER is needed.'
	echo -e '\t\t\taccept values:\n\t\t\trestore, testing, server, client,' 
	echo -e '\t\t\tserver_udp, client_udp, client_browser'
	echo -e '\t-x\tExecute'
	echo -e '\t-h\tshows this help text'
}



#### Main


if [ "$#" -eq 0 ]; then 
	clean_workspace
	copy_and_build
	config_terminator testing
	run
else 
	while getopts "cbxht:" opt; do
	    case $opt in
	        c) clean_workspace;;
			b) copy_and_build;;
			t) config_terminator $OPTARG;;
			x) run;;
			h) help_text;;
			\?) echo "Invalid option: -$OPTARG" >&2;;
			:) echo "Option -$OPTARG requires an argument." >&2;;
			*) echo "$OPTARG";;
	    esac
	done
	if [ $OPTIND -eq 1 -a "$#" -gt 0 ]; then 
		clean_workspace
		copy_and_build
		config_terminator $1
		run
	fi
fi




