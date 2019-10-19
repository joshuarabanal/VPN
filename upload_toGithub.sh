cd "/home/pi/Documents/github/VPN"
sudo git config --global credential.helper.cache
sudo git config --global user.email "joshuarabanal@gmail.com"
sudo git config --global user.name "joshuarabanal"
echo "insert your commit message for VPN"
read message
echo "fetching for any changes"
sudo git pull "https://joshuarabanal:SU0798ni@github.com/joshuarabanal/VPN.git"
echo "adding any new files"
sudo git add -A
echo "uploading changes"
sudo git commit -m "$message" -a

sudo git push


echo "pres enter to close"
read message