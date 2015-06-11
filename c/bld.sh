#!/bin/sh
# Builds the C documention for UMDoc

# Context Create
cd context_create/
if /home/sglass/projects/git/semlit/bin/semlit.sh context_create.sldoc; then :
	echo "Built Context-create"
	cd ../
else :
	echo "Error building Context-create"
	exit 1
fi

# Source Create
cd source_create/
if /home/sglass/projects/git/semlit/bin/semlit.sh source_create.sldoc; then :
	echo "Built Source Create"
	cd ../
else :
	echo "Error building Source Create"
	exit 1
fi

# Setting Attributes
cd setting_attributes/
if /home/sglass/projects/git/semlit/bin/semlit.sh setting_attributes.sldoc; then :
        echo "Built Setting Attributes"
        cd ../
else :
        echo "Error building Setting Attributes"
        exit 1
fi

# checking flight size after EWOULDBLOCK
cd check_flight_size_ewouldblock/
if /home/sglass/projects/git/semlit/bin/semlit.sh check_flight_size_ewouldblock.sldoc; then :
        echo "Built Check Flight Size EWOULDBLOCK"
        cd ../
else :
        echo "Error building Check Flight Size EWOULDBLOCK"
        exit 1
fi

