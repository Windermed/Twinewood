## Project Explanation

I’ve made this game using my own engine that I wrote from the last video and this time, I turned it into a submodule on the game’s repository which really made for good practice on learning how to use git more effectively (and it helps if I need to update the WinderEngine which changes I make to it along with the game)

I made a complete separate room (which is what i refer to areas in the game) that contains a menu utilizing the GameText class. This is the menu that you see when you boot up the game for the first time albeit it’s more so just there until I could make a proper menu in it’s place. This menu is it’s own separate class as well.

Then from then on, I used a separate JSON library to parse a json file that reads data from said JSON (in this case, the character table) all of this which has been added for WinderEngine and is a port from my other project. once the data is read, It is then passed to the StatBar class which is setup using a sprite and for every point that stat bar has, it gets filled using a default white sprite who’s color gets overridden depending on what its set to and this gives you the opportunity to configure it however you wish.

Outside of the stat screen though, you got this HUGE overworld map which I quickly put together when I begun working on this project. It’s still unfinished but it still serves well as a test map for what I wanted to show today. This is actually handled by the BaseRoom class in winderengine which handles loading the sprite of the room, scaling it, and allowing the player to walk through it.

then from then on you got the Player and enemy classes. for this, I actually made them inherit from the Character class and you can see that I’ve used inheritance and polymorphism with how these classes (and many that you’ll see) will often override functions from their parent class. One of the most common examples you can see is the Object class as many classes in this project (such as the character, trigger box, etc) will override their parent’s virtual functions.

for the player specifically here, I’ve actually wrote a separate class in addition to player called Animator which helps when it comes to animating sprites.

after this, I decided to do something new with the project and given how I had initially planned to have something be interacted with in the game I created an interface class called Interactable which is mostly comprised of virtual functions and the purpose of this class is so that if we want to have any objects in the game be interactable, all they would need to do is have that class inherit from it alongside object as well. You can see an example of this with the Trigger Box class I wrote which when interacted, will open up a dialogue box with dialogue that can be written to and displayed to the player.

and now finally, we get to what was the most challenging part I worked on while I was working on this project.. the battle system!!

Surprisingly, this turned out to be more challenging than I thought at first, not only because of my previous experience of creating battle UIs in console programs but also I had no idea how I wanted to do the battle system at the full scope.

I had ideas ranging from reusing what I did in Particles and turning the bullet hell mechanic into an attack (similar to another game) but given the amount of time I had I decided to stick with doing a turn based battle system instead. (although I will likely revisit this idea in the future just for the fun of it anyways)  buy for the battle system, I utilized CurrentState in TwineEngine to detect whether or not the player is in the world. if they are, we then use the CurrentBattle instance to then Start a battle which utilizes BattleSequence, the main class for the battle system.
With BattleSequence, we also have BattleMenu which is mainly just for the menu itself. it carries the background, buttons, etc.
