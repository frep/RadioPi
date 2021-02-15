#!/bin/bash
###################################################################################
#  file: post-installation.sh
# autor: frep
#  desc: Sets up the raspberry pi 4 for the radioPi project. 
#        This setup is based on top of a "volumino" distribution
#        Important: dont run script as superuser, just "./setup.sh"
###################################################################################
# paths and variables
###################################################################################

setupdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


###################################################################################
# functions
###################################################################################

function updateSystem {
    sudo apt-get update
    sudo apt-get -y upgrade
    sudo apt-get -y dist-upgrade
    sudo apt-get clean
    sudo apt-get -y autoremove
}

function finderScreenSharing {
	sudo apt-get install netatalk -y
	sudo cp -f ${setupdir}/vnc/rfb.service /etc/avahi/services/
	sudo cp -f ${setupdir}/vnc/afpd.service /etc/avahi/services/
}

function installMqttBroker {
    sudo apt-get install -y mosquitto mosquitto-clients
    sudo systemctl enable mosquitto.service
}


###################################################################################
# program
###################################################################################

#updateSystem
#finderScreenSharing
#installMqttBroker


###################################################################################
# unscripted modifications:
###################################################################################

# /boot/config.txt:
# =================
# hdmi_force_hotplug=1
# hdmi_group=2
# hdmi_mode=73

# /etc/dphys-swapfile:
# ====================
# CONF_SWAPSIZE=1024
