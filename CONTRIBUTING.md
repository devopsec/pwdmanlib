# Contributing to PwdManLib

These are some of the best best practices and guidelines that should be followed when contributing to the project.

## Create an Issue

- First, create an issue to let everyone know you would like to work on a section of code, bug, or add some new feature.
This allows a discussion to be opened and feedback / advise to given if necessary.
- Make sure to mention the <b>main</b> topic, bug, or feature in the title of the issue.

## Fork the Repo

Fork a clone of the repo to your github and clone the fork to your machine.<br>
This can be done using your preferred IDE (tutorial for Jetbrains Clion to come..) or using the git cmd:
```
git clone --recursive <your fork url>
```
Note: biicode is the required dependency tracker of choice for this project.

## Merging changes

- Make changes, and commit to your repo, putting <b>comments</b> inline as needed and giving a general description in commit message.
- Make a pull request on github or through git, your changes will be reviewed.
- If there are conflicts that need resolved, we will start a discussion and provide any help necessary.
- Then your changes will be merged in with the master repo and your name will be added to the contributions documentation (Thank You!)

## General guidelines

1. You <strong>must</strong> paste (in docstrings) the license information for this repo, in all source files. This project is licensed
under Apache License V2 and <strong>ALL</strong> contributors will receive credit for their work, no matter how small the contribution.<br>
The license info has been provided below for your convenience, add your name in the contributors section before committing.

```
    /********************************************************************************
     *                          Copyright 2017 DevOpSec                             *
     *                                                                              *
     *   Licensed under the Apache License, Version 2.0 (the "License");            *
     *   you may not use this file except in compliance with the License.           *
     *   You may obtain a copy of the License at                                    *
     *                                                                              *
     *       http://www.apache.org/licenses/LICENSE-2.0                             *
     *                                                                              *
     *   Unless required by applicable law or agreed to in writing, software        *
     *   distributed under the License is distributed on an "AS IS" BASIS,          *
     *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
     *   See the License for the specific language governing permissions and        *
     *   limitations under the License.                                             *
     *                                                                              *
     *      Contributors: ....                                                      *
     *                                                                              *
     ********************************************************************************/
```

2. Wrap your routines in custom wrappers as much as possible. We want cross-platform code, so we must make use of #ifdef
for platform-specific function calls / platform-specific #define

3. Make your code self-documenting, ```ts``` is not a good name for a timestamp variable. Rather, you should use identifiers that
another developer can take a glance at and understand what it does, ```epoch_timestamp``` on the other-hand is un-mistakable,
and your intention is crystal clear.

4. Provide inline documentation comments for all functions, wrappers, abstractions, and larger routines. Also, if a declaration
is not perfectly clear or calls external functions / routines add in a quick comment describing where and what the routine is calling.
  
## Test and Debugging

More to follow...

## Documentation

- There should be a ```README.md``` for each top level directory in this project as well as for each library created.
- Each ```README.md``` file should provide a brief overview of what's in that directory or library and guides on "how to".
- Provide instructions, and code examples on how to replicate what you are describing, or how to perform the operations.
Nobody likes having to email their coworkers to figure out what the code they just pulled down does, so please lend the 
same courtesies to your fellow dev's as you would if you were stuck next to them in cubical city all day.
- Although this is a library made for developers, assume that your documentation is written for a manager or someone from
sales trying to pitch the new features. By keeping to "soft-topics" instead of technical / "hard-topics" everyone benefits.
More detailed technical descriptions could be reference in a specific section for the people that need them.
- When creating issues for bugs, provide screen-shots, code snippets, and instructions on replicating the issue. 