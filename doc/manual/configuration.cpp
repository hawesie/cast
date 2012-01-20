s/**
   \page man_configuration Configuring A CAST System

\section config_intro Introduction

To run a CAST system you must provide the client with a description of
the components you wish to run and the way that they are divided
across subarchitectures. This configuration file is written in plain
text and called a <em>CAST file</em>. For the sake of consistency we
always give the config files a .cast suffix and store them in a
/config directory. In a CAST config file you can describe anything
from a single component, to a single subarchitecture (if you want to
test it on its own) or to a full architecture with any number of
subarchitectures. For each component you can specify the host it runs
on and a list of command-line arguments which as passed into the
component's configure method [\link
cast::CASTComponent::configure(const std::map<std::string,std::string>&) c++\endlink|\link cast::core::CASTComponent::configure(Map<String,String>) java\endlink].

\section config_format Formatting

\subsection config_comments Comments

Comment lines start with #.


\subsection config_host The HOST Line

If you want to specify a host machine to be used as the default for
all subsequent components, you can specify it using the keyword
<code>HOST</code>. E.g. The entry...

\verbatim
HOST dewey 
\endverbatim

... will cause the configuration code to assign all subsequent
components the hostname of dewey unless overridden.

   


\subsection config_subarch The SUBARCHITECTURE Section

The majority of a config file will probably be made up of
subarchitecture descriptions. A subarchitecture description is started
with the following line

\verbatim
SUBARCHITECTURE <subarchName> <opt:subarchHost>
\endverbatim

This states that the component descriptions following this line make
up the subarchitecture, and run on the optionally specified host. E.g.

\verbatim
SUBARCHITECTURE vision.sa louie
\endverbatim

Describes a subarchitecture <code>vision.sa</code> with its components
to run by default on <code>louie</code>. The lines following this
<em>must</em> describe a working memory component, and task manager
component and zero or more processing components.



\section config_wm The Working Memory Line

You describe a subarchitecture working memory with the following line...

\verbatim
<opt:host> <lang> WM <class> <opt:command line>
\endverbatim

Where <code><opt:host></code> is an optional name of the host to run
the component on, <code><lang></code> specifies the language the
working memory was written in and is either <code>CPP</code> or
<code>JAVA</code>, <code>WM</code> states that this component is a
working memory, <code><class></code> is the name of the component
library (for C++) or is a fully qualified class name (for Java), and
<code><opt:command line></code> is a list command parameters which are
passed to the component's configure method. These are described in
more detail \ref config_args "here". E.g.

\verbatim
JAVA WM cast.architecture.SubarchitectureWorkingMemory --debug -v 2 -N
\endverbatim

... specifies a java working memory component with some arguments, and 

\verbatim
hewey C++ WM SubarchitectureWorkingMemory --log
\endverbatim


... does the same for a C++ working memory (from
libSubarchtectureWorkingMemory.so), but also specifies that it runs on
a specific host.



\subsection config_tm The Task Manager Line

This is the same format as the working memory line, but with <code>TM</code>
instead of <code>WM</code>. E.g.

\verbatim
hewey JAVA TM cast.architecture.AlwaysPositiveTaskManager 
\endverbatim


\subsection config_um Unmanaged Component lines

Unmanaged components [\link cast::UnmanagedComponent c++\endlink|\link
cast::architecture::UnmanagedComponent java\endlink] are added to the
subarchitecture using a line of the following format...

\verbatim
<opt:host> <lang> UM <id> <class> <opt:command line>
\endverbatim

This is almost identical to preceding lines, with the addition of the
<code><id></code> argument which specifies the unique identifier of
the processing component (used to identify it at rutime). E.g.

\verbatim
CPP UM ernie.wise StraightMan --setup "what's brown and sticky"
\endverbatim


\subsection config_mg Managed Component lines

Managed components [\link cast::ManagedComponent c++\endlink|\link
cast::architecture::ManagedComponent java\endlink] are added in the same way as unmanaged components,
but with <code>MG</code> instead of <code>UM</code>. E.g.

\verbatim
JAVA MG eric.morecambe comedyarch.FunnyMan --punchline "a stick" --log #--debug
\endverbatim


\subsection CAST Components

You can also add components a CAST system which are not part of a
subarchitecture [\link cast::CASTComponent c++\endlink|\link
cast::core::CASTComponent java\endlink].

This is done in a similar way to other components, but with a
<code>COMPONENT</code> prefix...

\verbatim
COMPONENT <opt:host> <lang> <name> <class> <opt:command line>
\endverbatim

e.g.

\verbatim
COMPONENT CPP eg ExampleComponent --log  #--debug
\endverbatim

Would create an extra C++ component in the configuration.



\section config_args "Command Line" Configuration

The command line-style arguments given to any process are parsed into
string key-value pairs and are placed into the map given as input to
the component's configure method [\link cast::CASTComponent::configure(const zstd::map<std::string,std::string>&) c++\endlink|\link
cast::core::CASTComponent::configure(Map<String,String>)
java\endlink]. Any command line entry starting with <code>-</code> is
treated as a key and then the subsequent string is treated as its
associated value. Keys without attached values are given the value
"true". Explicitly marked strings are not broken up. E.g.

\verbatim
--debug -v 2 -N -say "Hello World"
\endverbatim

would result in the key-value pairs:

\verbatim
--debug = true
-v = 2 
-N  = true
-say = Hello World
\endverbatim

\section config_example Example Configuration

\verbatim
HOST localhost

SUBARCHITECTURE stage.subarch 
CPP WM SubarchitectureWorkingMemory  --log --debug
CPP TM AlwaysPositiveTaskManager #--log --debug
JAVA UM straight.man comedyarch.StraightMan -s "what's brown and sticky" --log --debug
CPP MG funny.man FunnyMan --punchline "a stick" --log --debug

SUBARCHITECTURE audience.subarch 
CPP WM SubarchitectureWorkingMemory  --log --debug
CPP TM AlwaysPositiveTaskManager #--log --debug
JAVA MG audience.member comedyarch.AudienceMember --reaction "YAY!" --log --debug


SUBARCHITECTURE director.subarch
#CPP WM SubarchitectureWorkingMemory  --log --debug
JAVA WM cast.architecture.SubarchitectureWorkingMemory  --log --debug
CPP TM AlwaysPositiveTaskManager #--log --debug
JAVA MG ass.director comedyarch.AssistantDirector --log #--debug
JAVA MG director comedyarch.Director --audience audience.subarch --log  #--debug
\endverbatim


\section config_includes Including parts of configuration files

Configuration files may grow in a complex system. In this case one would split
the configuration file in multiple parts and include the parts in the main
file. This can be done with the <code>INCLUDE</code> statement:

\verbatim
# ----- file: main.cast -----
INCLUDE  stage_subarch.cast

# ----- file_ stage_subarch.cast
SUBARCHITECTURE stage.subarch 
CPP WM SubarchitectureWorkingMemory  --log --debug
CPP TM AlwaysPositiveTaskManager #--log --debug
JAVA UM straight.man comedyarch.StraightMan -s "what's brown and sticky" --log --debug
CPP MG funny.man FunnyMan --punchline "a stick" --log --debug
\endverbatim

The <code>INCLUDE</code> statement accepts a file path that is relative to the
directory in which the currently processed file resides.

\section config_variables Setting and using variables
(SETVAR, VARDEFAULT introduced in version 2.1.13)

Sometimes multiple components use the same setting which has to be entered in
the arguments of every component. To simplify the modification of such
arguments, variables can be used with the keywords <code>SETVAR</code> and
<code>VARDEFAULT</code>. With <code>SETVAR</code> the value of a variable is
changed unconditionally while <code>VARDEFAULT</code> sets the value of a
variable only if the variable was not defined before.

\verbatim
SETVAR punchline="a brown stick"
VARDEFAULT punchline="a stick"
VARDEFAULT string="what's brown and sticky"
\endverbatim

In this case the variable <code>punchline</code> has the value <code>"a brown
stick"</code> and the value of <code>string</code> is <code>"what's brown and
sticky"</code>. Note that everything after <code>=</code> is part of the
variables value, including the quotes.

The value of the variable can be used in the argument part of a configuration
line with the variable expansion expression <code>%(varname)</code>:

\verbatim
JAVA UM straight.man comedyarch.StraightMan -s %(string) --log --debug
CPP MG funny.man FunnyMan --punchline %(punchline) --log --debug
\endverbatim

Variables can span multiple lines if the right-hand-side of the variable
definition is <code>&lt;multiline&gt;</code>. The value of the variable spans
until the line that contains only <code>&lt;/multiline&gt;</code>:

\verbatim
SETVAR punchline=a brown stick
VARDEFAULT string=what's brown and sticky

SETVAR straight_params=<multiline>
   -s "%(string)"
   # --log --debug
</multiline>
JAVA UM straight.man comedyarch.StraightMan %(straight_params)

SETVAR funny_params=<multiline>
   --punchline "%(punchline)"
   --log --debug
</multiline>
CPP MG funny.man FunnyMan %(funny_params)
\endverbatim

The lines that are inside the <code>&lt;multiline&gt;</code> section are
concatenated with a single space except for the lines that start with
<code>#</code> which are ignored.

This example also shows that the value of a variable can be used inside the
right-hand-side of another variable.


\subsection config_special_vars Special variables

The configuration parser sets the following variables every time a new
configuration file is included:

<ul>
   <li>CONFIG_DIR - the directory of the main configuration file
   <li>CURRENT_DIR - the directory of the current (included) configuration file
   <li>CURRENT_FILE - the filename of the current (included) configuration file
</ul>

This variables can be used to reference extra files needed by some components
with relative filenames:

\verbatim
SETVAR camera_config=%(CURRENT_DIR)/config/camera.ini
\endverbatim

\section config_distributed Distributed execution
(HOSTNAME introduced in version 2.1.13)

As stated above every component can be executed on a different host which can be
set with the HOST statement or with the address of the host in a component
definition command. Defining hosts in such a way is very non-portable.

To make the configuration files more portable, the command
<code>HOSTNAME</code> can be used. The command must be placed in a
configuration file before any other command except <code>SETVAR</code>,
<code>VARDEFAULT</code> and <code>INCLUDE</code> (which can again include only
the four commands mentioned here).

The defined host-name can be used in the form <code>[host-name]</code> anywhere
a host address is recognized by the config parser, as described with each
command above. A host name can additionally be used in a variable expansion
expression as <code>%(host:host-name)</code>.

In the following case the component <code>laser.server</code> is started on
<code>localhost</code> (<code>LaserHost -&gt; Main -&gt; localhost</code>)
while the component <code>robot.server</code> starts on the host
<code>PlayerHost</code> (<code>192.168.26.34</code>). Both components connect
to an instance of the Player server which is also running on the host
<code>PlayerHost</code>.

\verbatim
HOSTNAME   Main        localhost
HOSTNAME   PlayerHost  192.168.26.34
HOSTNAME   LaserHost   [Main]

HOST       [Main]
COMPONENT  [LaserHost] CPP laser.server LaserServerPlayer --player-host %(host:PlayerHost)

COMPONENT  [PlayerHost] CPP robot.server RobotbaseServerPlayer --player-host %(host:PlayerHost)
\endverbatim

\section conditional_blocks Conditional Blocks
(IFTRUE, IFFALSE, IFEQ, IFNEQ, IFOPTALL, IFOPTANY introduced in version 2.1.16b?)

A conditional block adds other directives inside the block to the final
configuration only if the condition of the conditional directive that starts
the block is fulfilled.  Conditional directives accept one or two parameters
split by a comma ','.  The variables in the parameters are expanded and the
resulting parameter value is trimmed (leading and trailing whitespace is
removed).  Finally the parameters are evaluated depending on the type of the
conditional directive.

A conditional block starts with a conditional directive (IFxxx) and is
terminated by an ENDIF directive.  An optional ELSE directive may be present
before the ENDIF directive.  Conditional blocks may be nested to an arbitrary
depth.  Every directive must be on its own line.  Directives are not searched
inside <code>&lt;multiline&gt;</code> blocks (see \ref config_variables
"variables").

\verbatim
IFTRUE(yes)
   IFEQ(%(value), 11)
   ELSE
   ENDIF
ENDIF
\endverbatim

\subsection conditional_directives Conditional Directives

\subsubsection IFTRUE, IFFALSE

\verbatim
IFTRUE(value)
IFFALSE(value)
\endverbatim

<code>IFTRUE(value)</code> succeeds if the value is one of <code>true</code>,
<code>yes</code>, <code>on</code>, or a digit <code>1</code> to
<code>9</code>. It doesn't succeed if the value is one of
<code>false</code>, <code>no</code>, <code>off</code>, <code>0</code> or
empty. Any other value is invalid.

<code>IFFALSE(value)</code> succeeds when value is valid and
<code>IFTRUE(value)</code> doesn't succeed.


\subsubsection IFEQ, IFNEQ

\verbatim
IFEQ(value1, value2)
IFNEQ(value1, value2)
\endverbatim

<code>IFEQ(value1, value2)</code> succeeds when then values are equal strings.
<code>IFNEQ(value1, value2)</code> succeds when the values are different
strings.

\subsubsection IFOPTALL, IFOPTANY

\verbatim
IFOPTALL(value, opt_flags)
IFOPTANY(value, opt_flags)
\endverbatim

The parameters <code>value</code> and <code>opt_flags</code> are each treated
as a whitespace delimited list.  Each item in a list represents a flag that can
be either set (<code>flag</code>) or unset (<code>!flag</code>). If a flag is
not in a list it is considered unset.

<code>IFOPTALL(value, opt_flags)</code> succeeds if ALL flags from
<code>opt_flags</code> have the same state (set/unset) in value.

\verbatim
SETVAR value=a b
IFOPTALL(%(value), a)       --> true
IFOPTALL(%(value), a b c)   --> false (c is not in value)
IFOPTALL(%(value), a b !c)  --> true  (c is not in value, so !c is assumed)
\endverbatim

<code>IFOPTANY(value, opt_flags)</code> succeeds if ANY flag from
<code>opt_flags</code> has the same state (set/unset) in value.

\verbatim
SETVAR value=a b
IFOPTANY(%(value), a)       --> true
IFOPTANY(%(value), a b c)   --> true  (a is present in value)
IFOPTANY(%(value), c)       --> false
IFOPTANY(%(value), !c)      --> true  (c is not in value, so !c is assumed)
\endverbatim


*/
