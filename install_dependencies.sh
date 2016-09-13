#!/bin/bash

if (( $EUID != 0 )); then
    echo "Please, run as root!"
    exit
fi

# For Linux:

echo "installing C++ environment for the Physical layer..."
apt-get install build-essential

echo "Installing PHP environment for the Network layer..."
apt-get install php5 apache2

echo "Installing Python environment for the Transport layer..."
apt-get install python3


echo "Installing Scala environment for the Transport layer..."
add-apt-repository ppa:webupd8team/java
apt-get update
apt-get install oracle-java8-installer
apt-get install oracle-java8-set-default
echo "Java installed successfully."

wget http://downloads.lightbend.com/scala/2.11.8/scala-2.11.8.tgz
wget http://www.scala-lang.org/files/archive/scala-2.11.8.deb
dpkg -i scala-2.11.8.deb
apt-get update
apt-get install scala
rm scala-2.11.8.deb
echo "Scala installed successfully."

echo "deb https://dl.bintray.com/sbt/debian /" | sudo tee -a /etc/apt/sources.list.d/sbt.list
apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 2EE0EA64E40A89B84B2DF73499E82A75642AC823
apt-get update
apt-get install sbt
echo "Scala-sbt installed successfully."

echo "Setting Scala environment..."
#export JAVA_HOME=/usr/local/java-current
export $SCALA_HOME /usr/local/share/scala
export $PATH $PATH:$SCALA_HOME/bin
echo "Done."


