# ttymato: Pomodoro in your terminal

Heavily based on [ttyclock](https://github.com/xorg62/tty-clock), **ttymato** is
a terminal application to manage your pomodoro sessions.

At the moment, it provides the following features:
* ncurses display *à la* tty-clock
* manage your pomodoro inside a terminal window (pause, continue, next pomodori
  and so on)
* notifications and terminal bell to get your attention at the end of intervals

# Licence
BSD License. See Licence.md file.

# Usage

	usage: ttymato [-hunbN] [-D <pomodoro>,<break>,<longbreak>] [-p number] 
		 -h 	 Print this help                                        
		 -c 	 Path to configuration file 							
		 -u 	 Urgent bell on intervals end                           
		 -n 	 Notification on intervals end                          
		 -N 	 Don't quit on keypress                                 
		 -b 	 Enable blinking colon                                  
		 -D 	 Duration of pomodoro, break and longbreak (in minutes) 
		 -p 	 Number of pomodori 

# Screenshot
![Screenshot](https://raw.github.com/driquet/ttymato/master/screenshots/screenshot_0.png)

# Configuration file
Instead of using command line arguments, you can set ttymato parameters in its
configuration file. You can specify the path of this file using command line
argument `-c`. Otherwise, it will look for the configuration file at the
following locations:
* first `$XDG_CONFIG_HOME/ttymato/config`
* then `$HOME/.config/ttymato/config`
* and `$HOME/ttymato.conf`

Configuration file syntax described in the manpage.

# TODO
* manpage
* server-thread and cli client to get the status and interact with ttymato
* display commands ("p=pause n=next ...") at the top or the bottom
* action to start over the cycle at the beginning
* options to ask confirmation when doing an irreversible action (quitting, starting over, and so on)
* enable color support
