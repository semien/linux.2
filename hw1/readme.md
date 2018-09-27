# Task 1. Kernel-mode phone book

### Create character device:
```sh
chmod +x make_device.sh
./make_device.sh
```
### Delete character device:
```sh
chmod +x remove_device.sh
./remove_device.sh
```
### Using phone book:
```sh
gcc mediator.c -o med
sudo ./med
```

### Commands:
+ add record
```sh
> add first_name last_name phone_number email age
```
+ delete all records with last_name
```sh
> delete last_name
```
+ print all records with last_name
```sh
> info last_name
```
+ exit
```sh
> exit
```
