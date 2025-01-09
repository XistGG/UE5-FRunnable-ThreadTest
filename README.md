# UE5-FRunnable-ThreadTest

This is a simple little project with functional `FRunnable` threads.

To try it out:

- Compile in your favorite IDE
- Launch `UEditor`
- Start PIE

Once in PIE, you'll see this widget at the top left of the viewport:

[![Control Widget](./blob/main/Images/StartStopWidget.png)](./blob/main/Images/StartStopWidget.png)

You can change the number of threads to whatever you want and click the `Start` button to spawn the threads.

The threads "work" by sleeping for a random 0.5 - 2.5 seconds.
After working, there is a 90% chance for the thread to pretend there is more work to do.
Thus, all threads will eventually randomly decide they are done working.

At any time you can click the `Stop` button to instruct the threads to exit ASAP.

This widget is super simple and doesn't do anything interesting.
It only interfaces with the `AXistPlayerController` C++ class, which is what does the interesting stuff.

## Interesting C++

### `FRunnableNonsense`

Source:
&#91;
[cpp](./blob/main/Source/ThreadTest/RunnableNonsense.cpp) |
[h](./blob/main/Source/ThreadTest/RunnableNonsense.h)
&#93;

This is the actual `FRunnable` thread class, the central idea explored in this project.

### `AXistPlayerController`

Source:
&#91;
[cpp](./blob/main/Source/ThreadTest/XistPlayerController.cpp) |
[h](./blob/main/Source/ThreadTest/XistPlayerController.h)
&#93;

The Player Controller is what actually manages the threads in this project.
You can ignore most of it, except for these interesting methods:

#### `AXistPlayerController::StartNonsense(Num)`

- Called by the widget to start `Num` `FRunnableNonsense` threads
  - Invoked by the `Start` button

#### `AXistPlayerController:StopNonsense()`

- Called by the widget to stop all running `FRunnableNonsense` threads
  - Invoked by the `Stop` button

#### `AXistPlayerController::Tick()`

- Detects `FRunnableNonsense` threads that have exited and cleans up the memory associated with them

#### `AXistPlayerController::EndPlay()`

- Cleans up any remaining `FRunnableNonsense` threads that are still running

## Widget BP

[![Widget BP](./blob/main/Images/WidgetBP.png)](./blob/main/Images/WidgetBP.png)

The widget BP is super simple, all it does is call the above methods
in the `AXistPlayerController`.
