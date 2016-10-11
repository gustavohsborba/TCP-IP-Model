#!/bin/bash


#Coloring bash:
R='\e[31m' #RED
NC='\e[00;39m' #NO COLOR
G='\e[32m' #GREEN
B='\e[01;39m' #BOLD


if (( $EUID != 0 )); then
    echo "Please, run as root!"
    exit
fi

function pause {
    read -n1 -rsp $'Press any key to continue...\n'
}


# For Linux:

echo "${B}Installing C++ environment for the Physical layer... $NC"
apt-get install build-essential

echo "${B}Installing PHP environment for the Network layer...$NC"
apt-get install php5 apache2

echo "${B}Installing Python environment for the Transport layer...$NC"
apt-get install python3


echo "${B}Installing Scala environment for the Transport layer...$NC"
add-apt-repository ppa:webupd8team/java
apt-get update
apt-get install oracle-java8-installer
apt-get install oracle-java8-set-default
echo "Java installed successfully."


if [! -f 'scala-2.11.8.tgz']; then
    wget http://downloads.lightbend.com/scala/2.11.8/scala-2.11.8.tgz
fi

if [! -f 'scala-2.11.8.deb']; then
    wget http://www.scala-lang.org/files/archive/scala-2.11.8.deb
fi 
dpkg -i scala-2.11.8.deb
apt-get update
apt-get install scala
rm scala-2.11.8.deb
rm scala-2.11.8.*
echo "${B}Scala installed successfully.$NC"

if [! -f '/etc/apt/sources.list.d/sbt.list']; then
	echo "deb https://dl.bintray.com/sbt/debian /" | sudo tee -a /etc/apt/sources.list.d/sbt.list
    apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 2EE0EA64E40A89B84B2DF73499E82A75642AC823
fi 
apt-get update
apt-get install sbt
echo "${B}Scala-sbt installed successfully.$NC"

echo "${B}Setting Scala environment...$NC"
#export JAVA_HOME=/usr/local/java-current
export $SCALA_HOME /usr/local/share/scala
export $PATH $PATH:$SCALA_HOME/bin
echo "${B}Done.$NC"

