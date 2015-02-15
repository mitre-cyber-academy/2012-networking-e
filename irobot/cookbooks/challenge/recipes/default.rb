bash "initd" do
  user "root"
  code <<-EOH
  
  # move initd
  mv /home/ubuntu/chef-solo/cookbooks/challenge/aux/init_d_script /etc/init.d/challenge
  chmod 755 /etc/init.d/challenge
  
  # register it with boot sequence
  update-rc.d challenge defaults 98 02
  
  #process limit ubuntu
  echo "ubuntu hard nproc 512" >> /etc/security/limits.conf
  
  EOH
end

bash "challenge" do
  user "ubuntu"
  code <<-EOH
  
  # all challenge files
  mkdir $HOME/challenge
  mv /home/ubuntu/chef-solo/cookbooks/challenge/aux/* $HOME/challenge
  
  cd $HOME/challenge
  gcc src.c -o server1
  gcc src2.c -o server2
  
  
  #start the service
  sudo /etc/init.d/challenge start
  
  #reboot machine
  #sudo shutdown -r now

  EOH
end
