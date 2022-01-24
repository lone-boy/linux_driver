source_name := 24lc04b
PROJECT_NAME := eeprom


all:
	arm-linux-gnueabihf-gcc $(source_name).c -o $(PROJECT_NAME)
	sudo cp $(PROJECT_NAME) /tftpboot/nfsroot/home/root/
clean:
	sudo rm -f /tftpboot/nfsroot/home/root/$(PROJECT_NAME)
	sudo rm -f $(PROJECT_NAME)