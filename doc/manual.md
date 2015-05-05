% YAMSS User's Guide

# Table of Contents

* [Synopsis](#synopsis)
* [Description](#description)
* [Options](#options)
* [Input File](#input-file)
    - [Structure](#structure)
        + [Nodes](#nodes)
        + [Elements](#elements)
        + [Grids](#grids)
    - [Modes](#modes)
        + [Nodal Displacements](#nodal-displacements)
        + [Shape Functions](#shape-functions)
    - [Equations of Motion](#equations-of-motion)
    - [Loads](#loads)
        + [Lua Evaluator](#lua-evaluator)
    - [Solution](#solution)
        + [Newmark-$\beta$ Method](#newmark-beta-method)
        + [Generalized-$\alpha$ Method](#generalized-alpha-method)
    - [Outputs](#outputs)
        + [Modes Filter](#modes-filter)
        + [Motion Filter](#motion-filter)
        + [Property Tree Filter](#property-tree-filter)
        + [Summary Filter](#summary-filter)
* [Nomenclature](#nomenclature)

# Synopsis

yamss [*options*] [*input-file*]

# Description

YAMSS (Yet Another Mode-based Structural Solver) is a program for solving the
modal form of the equations of motion of a vibrating system with multiple
degrees of freedom.  The general governing system of ordinary differential
equations can be written as

$$
\left[\tilde{M}\right]\left\{\ddot{x}\right\}
+ \left[\tilde{C}\right]\left\{\dot{x}\right\}
+ \left[\tilde{K}\right]\left\{x\right\}
= \left\{\tilde{F}\right\},
$$

where $\left\{x\right\}$ is a time-dependent vector of the $n$ degrees of
freedom, $\left\{\tilde{F}\right\}$ is a vector of applied forces, and
$\left[\tilde{M}\right]$, $\left[\tilde{C}\right]$, and
$\left[\tilde{K}\right]$ are symmetric matrices referred to respectively as
the mass, damping, and stiffness matrices.  It can be useful to transform
$\left\{x\right\}$ as follows:

$$
\left\{x\right\} = \left[\Psi\right]\left\{q\right\}.
$$

Here $\left[\Psi\right]$ are the eigenvectors of the system.  Under this
transformation, the governing equations become

$$
\left[\Psi\right]^T\left[\tilde{M}\right]\left[\Psi\right]\left\{\ddot{q}\right\}
+ \left[\Psi\right]^T\left[\tilde{C}\right]\left[\Psi\right]\left\{\dot{q}\right\}
+ \left[\Psi\right]^T\left[\tilde{K}\right]\left[\Psi\right]\left\{q\right\}
+ \left[\Psi\right]^T\left\{\tilde{F}\right\}
$$

or, more briefly,

$$
\left[M\right]\left\{\ddot{q}\right\}
+ \left[C\right]\left\{\dot{q}\right\}
+ \left[K\right]\left\{q\right\}
= \left\{F\right\}.
$$

In this expression,

$$
\begin{aligned}
  M &= \left[\Psi\right]^T\left[\tilde{M}\right]\left[\Psi\right] \\
  C &= \left[\Psi\right]^T\left[\tilde{C}\right]\left[\Psi\right] \\
  K &= \left[\Psi\right]^T\left[\tilde{K}\right]\left[\Psi\right] \\
  F &= \left[\Psi\right]^T\left\{\tilde{F}\right\}.
\end{aligned}
$$

If a finite set of $m$ mode shapes $\left[\Phi\right]$ are used in place of the
full set of eigenvectors $\left[\Psi\right]$, then:

$$
\left[M\right]\left\{\ddot{q}\right\}
+ \left[C\right]\left\{\dot{q}\right\}
+ \left[K\right]\left\{q\right\}
\approx \left\{F\right\}.
$$

YAMSS solves this set of ordinary differential equations for the time history
of the modal coordinates $\left\{q\right\}$.  The governing equations are
approximate when $m < n$, but can be quite accurate, even when $m \ll n$.

# Options

`-c`, `--complex`
:   Enable complex mode.  This mode can be used to calculate the derivative of
    output quantities with respect to changes in a single input quantity.

`-h`, `--help`
:   Print a help message and exit.  The help message prints the synopsis and
    this list of options.

`-l`, `--legal`
:   Print a legal statement and exit.  The legal statement includes the version
    number, a copyright statement, and a description of the software license.

`-v`, `--version`
:   Print the version and exit.

# Input File

YAMSS reads an XML file to define a model, configure the numerical algorithm
used to integrate the governing equations, and set output options.  The root
element of this file must be `<yamss>` and it must contain six children, as
shown below.  They can appear in any order.  The contents of each subelement
are described in the following subsections.

```xml
<?xml version="1.0"?>
<yamss>
    <structure></structure>
    <modes></modes>
    <eom></eom>
    <loads></loads>
    <solution></solution>
    <outputs></outputs>
</yamss>
```

## Structure

The equations of motion are used to describe the motion of a structure, which
is defined by a set of points called nodes.  The nodes can be connected to
form elements.

### Nodes

Each node has the following properties:

* `id` -- identification number (type: $\mathbb{N}_0$, required)
* `x` -- x-coordinate of the point (type: $\mathbb{R}$, default: 0)
* `y` -- y-coordinate of the point (type: $\mathbb{R}$, defualt: 0)
* `z` -- z-coordinate of the point (type: $\mathbb{R}$, default: 0)

Two nodes must not share the same identification number.  While the numbers
must be unique, they do not need to be sequential.  The nodal coordinates are
not used when integrating the equations of motion.  They may be used for
visualization or for interacting with an external program that provides loads.

### Elements

Nodes can be connected to form elements.  External loads are applied to
elements.  They may also be used for visualization purposes.  The following
element types are supported:

* `point` -- 0D point element, $p = 1$
* `line` -- 1D line element, $p = 2$
* `tria` -- 2D triangular element, $p = 3$
* `quad` -- 2D quadrilateral element, $p = 4$

In addition to its type, each element has an the following properties:

* `id` -- identification number (type: $\mathbb{N}_0$, required)
* `v` -- reference to a node (type: $\mathbb{N}_0$, required)

As with the nodes, each element identification number should be unique amongst
all of the elements, but need not be sequential.  There must be as many node
references elements as vertices in the element.

![Element Types](images/elements "Element Types")

### Grids

A two-dimensional grid of quadrilateral elements can easily be generated by
using a `<grid>` element.  It accepts the following parameters:

* `id` -- identification number of the first node and first element
          (type: $\mathbb{N}_0$, required)
* `u` -- number of nodes connecting the first and second vertices
         (type: $\mathbb{N}_1$, default: 11)
* `v` -- number of nodes connecting the second and third vertices
         (type: $\mathbb{N}_1$, default: 11)
* `vertices` -- should contain four `vertex` elements

Note that `u` and `v` must each have a value greater than or equal to 2.  Each
`<vertex>` element contains the following properties:

* `x` -- x-coordinate of the vertex (type: $\mathbb{R}$, default: 0)
* `y` -- y-coordinate of the vertex (type: $\mathbb{R}$, default: 0)
* `z` -- z-coordinate of the vertex (type: $\mathbb{R}$, default: 0)

The vertices must be specified in the same order as for a quadrilateral
element, as shown in the figure above.  The $u\times v$ nodes are numbered
sequentially, beginning with `id`.  The $(u - 1)\times(v - 1)$ quadrilateral
elements are also numbered sequentially, beginning with `id`.

### Examples

Within the XML file, the `<structure>` element contains a single `<nodes>`
child element that itself contains any number of `<node>` elements.  There
may also be an `<elements>` element that contains any number of children of
the following types: `<point>`, `<line>`, `<tria>`, and `<quad>`.  The
following XML fragment, for example, defines four nodes that form the corners
of a unit square lying in the $z = 0$ plane.  These nodes form the vertices of
two triangular elements.

```xml
<structure>
    <nodes>
        <node><id>1</id></node>
        <node><id>2</id><x>1.0</x></node>
        <node><id>3</id><x>1.0</x><y>1.0</y></node>
        <node><id>4</id><y>1.0</y></node>
        ...
    </nodes>
    <elements>
        <tria><id>1</id><v>1</v><v>2</v><v>3</v></tria>
        <tria><id>2</id><v>3</v><v>4</v><v>1</v></tria>
    </elements>
</structure>
```

The `<structure>` can also contain a single `<grids>` child element that
itself contains any number of `<grid>` elements.  The following fragment
demonstrates the use of `<grid>` to construct a rectangular grid of
quadrilateral elements, with 4 elements running along the x-axis and 2 running
along the y-axis.  The four corners of the grid are $(0, 0, 0)$, $(2, 0, 0)$,
$(2, 1, 0)$, and $(0, 1, 0)$.  The 15 nodes are numbered sequentially from 100
to 114 and the 8 elements are numbered from 100 to 107.

```xml
<structure>
    <grids>
        <grid>
            <id>100</id>
            <u>5</u>
            <v>3</v>
            <vertices>
                <vertex></vertex>
                <vertex><x>2.0</x></vertex>
                <vertex><x>2.0</x><y>1.0</y></vertex>
                <vertex><y>1.0</y></vertex>
            </vertices>
        </grid>
    <grids>
</structure>
```

## Modes

The mode shapes $\left[\Phi\right]$ are required to compute the generalized
forces $F$ from the applied loads $\tilde{F}$.  Each mode shape can be defined
in either one of two ways:

* Nodal displacments
* Shape functions

These methods are described below.

### Nodal Displacements

For each of the $m$ modes, the following properties are defined at each node:

* `id` -- reference to an identification number (type: $\mathbb{N}_0$, required)
* `x` -- translation along the x-coordinate (type: $\mathbb{R}$, default: 0)
* `y` -- translation along the y-coordinate (type: $\mathbb{R}$, default: 0)
* `z` -- translation along the z-coordinate (type: $\mathbb{R}$, default: 0)
* `p` -- rotation about the x-axis (type: $\mathbb{R}$, default: 0)
* `q` -- rotation about the y-axis (type: $\mathbb{R}$, default: 0)
* `r` -- rotation about the z-axis (type: $\mathbb{R}$, default: 0)

The identification number refers to a node defined within the `<structure>`
element.  The remaining properties define the mode shape at that node.  The
rotational coordinates are typically zero and can be left out of the input
file.

Within the XML file, the `<modes>` element should contain one `<mode>` element
for each of the $m$ modes.  Each `<mode>` must contain a single `<nodes>`
element that, in turn, contains any number of `<node>` elements.  Nodes not
referenced are assumed to have zero displacement.  The following fragment
defines two modes $\phi_1 = (x, y, 0)$ and $\phi_2 = (0, 0, x + y)$ for the
unit square defined above.  This example uses default values to the maximum
extent possible.

```xml
<modes>
    <mode>
        <nodes>
            <node><id>2</id><x>1.0</x></node>
            <node><id>3</id><x>1.0</x><y>1.0</y></node>
            <node><id>4</id><y>1.0</y></node>
            ...
        </nodes>
    </mode>
    <mode>
        <nodes>
            <node><id>2</id><z>1.0</z></node>
            <node><id>3</id><z>2.0</z></node>
            <node><id>4</id><z>1.0</z></node>
            ...
        </nodes>
    </mode>
    ...
</modes>
```

### Shape Functions

This method can be used if a mode shape can be described by a closed-form
analytic function.  The following parameters are defined for each mode that is
defined in this way:

* `type` -- function evaluator (type: string, default: `lua`)
* `parameters` -- a set of parameters that are passed to the function evaluator

A Lua evaluator is the only function evaluator currently supported.  [Lua][lua]
is a fast, lightweight, embeddable scripting language and is used here to
interpret and evaluate strings containing expressions for the mode shape
displacements.  It takes as parameters a set of expressions:

* `x` -- Displacement along the x-coordinate (type: string, default: 0)
* `y` -- Displacement along the y-coordinate (type: string, default: 0)
* `z` -- Displacement along the z-coordinate (type: string, default: 0)

Each string should conform to the syntax of a Lua expression.  The following
variables can be used inside these expressions:

* `x` -- the x-coordinate of the point
* `y` -- the y-coordinate of the point
* `z` -- the z-coordinate of the point

The following XML fragment provides an example.  The first two mode shapes are
defined as:

$$
\begin{aligned}
  \phi_1 &= \left[\sin(\pi x)\cdot\sin(\pi y)\right]\hat{k} \\
  \phi_2 &= \left[\sin(2\pi x)\cdot\sin(\pi y)\right]\hat{k}.
\end{aligned}
$$

```xml
<modes>
    <mode>
        <shape>
            <type>lua</type>
            <parameters>
                <expressions>
                    <z>math.sin(math.pi * x) * math.sin(math.pi * y)</z>
                </expressions>
            </parameters>
        </shape>
    </mode>
    <mode>
        <shape>
            <type>lua</type>
            <parameters>
                <expressions>
                    <z>math.sin(2.0 * math.pi * x) * math.sin(math.pi * y)</z>
                </expressions>
            </parameters>
        </shape>
    </mode>
    ...
</modes>
```

## Equations of Motion

The `<eom>` element in the XML file should contain two children: `<matrices>`
and `<initial_conditions>`.  The elements below can be used inside `<matrices>`
to define the generalized mass, damping, and stiffness matrices.

* `mass` -- Generalized mass matrix $\left[M\right]$ (type:
            $\mathbb{R}^{m\times m}$, default: $\left[1\right]$)
* `damping` -- Generalized damping matrix $\left[C\right]$ (type:
               $\mathbb{R}^{m\times m}$, default: $\left[0\right]$)
* `stiffness` -- Generalized stiffness matrix $\left[K\right]$ (type:
                 $\mathbb{R}^{m\times m}$, default: $\left[1\right]$)

The initial values and time rates of change of the generalized coordinates can
be set using the following children of `<initial_conditions>`:

* `displacement` -- Initial values of the modal coordinates,
                    $\left\{q(0)\right\}$ (type: $\mathbb{R}^m$, default:
                    $\left\{0\right\}$)
* `velocity` -- Initial values of the modal velocities,
                $\left\{\dot{q}(0)\right\}$ (type: $\mathbb{R}^m$, default:
                $\left\{0\right\}$)

Vectors are specified as a space-separated list of values.  Matrices are input
as a space-separated list of values with semicolons used to separate rows.  The
following fragment sets up the left-hand side of the governing equations and
initial conditions for a two degree-of-freedom system.

```xml
<eom>
    <matrices>
        <damping>0.05 0 ; 0 0.25</damping>
        <stiffness>4 0 ; 0 16</stiffness>
    </matrices>
    <initial_conditions>
        <displacement>1 0</displacement>
        <velocity>0 4</velocity>
    </initial_conditions>
</eom>
```

A shorthand notation is available for specifying diagonal matrices.  The
fragment below uses the `diag()` function to define the same damping and
stiffness matrices as in the previous fragment.

```xml
<eom>
    <matrices>
        <damping>diag(0.05 0.25)</damping>
        <stiffness>diag(4 16)</stiffness>
    </matrices>
    <initial_conditions>
        <displacement>1 0</displacement>
        <velocity>0 4</velocity>
    </initial_conditions>
</eom>
```

## Loads

Any number of external loads can be applied to the structure; these are summed
to yield the total applied load vector $\left\{\tilde{F}\right\}$:

$$\left\{\tilde{F}\right\} = \sum\left\{\tilde{f_i}\right\}.$$

The generalized force vector $\left\{F\right\}$ is calculated from
$\left\{\tilde{F}\right\}$ and the mode shapes $\left[\Phi\right]$ according to
the equation $\left\{F\right\} = \left[\Phi\right]^T\left\{\tilde{F}\right\}$.
Each load has the following properties:

* `id` -- identification number (type: $\mathbb{N}_0$, required)
* `type` -- evaluator type (type: string, required)
* `parameters` -- evaluator-dependent properties (optional)
* `elements` -- defines the elements on which the load is applied (required)

Two loads must not share the same identification number.  While the numbers
must be unique, they do not need to be sequential.  The type must be one of the
following, each of which is described in greater detail below.

* `lua` -- Lua evaluator

Each load is applied to a subset of the elements in the structure.  This
element set is formed by including any combination of the following tags under
the load's `<elements>` entry in the input file:

* `element` -- add a single element to the load set (type: $\mathbb{N}_0$, required)
* `range` -- add a range of elements to the load set (see below)
* `all` -- add all elements to the load set (type: empty)

The `range` tag includes the following two parameters:

* `begin` -- lower bound on the range (type: $\mathbb{N}_0$, default: 0)
* `end` -- upper bound on the range (type: $\mathbb{N}_0$, default: largest ID)

Because of the default values, an empty `<range />` is equivalent to `<all />`.

### Example

The following XML fragment serves as an example.  Here, a uniform vertical and
sinusoidally varying load is applied to two elements in the structure.

```xml
<loads>
    <load>
        <id>1</id>
        <type>lua</type>
        <parameters>
            <expressions>
                <z>math.sin(t)</z>
            </expressions>
        </parameters>
        <elements>
            <range>
                <begin>1</begin>
                <end>2</end>
            </range>
        </elements>
    </load>
    ...
</loads>
```

### Lua Evaluator

[Lua][lua] is a fast, lightweight, embeddable scripting language.  External
loads that take the form of simple, closed-form, analytic expressions can be
applied to the structure by using a Lua evaluator.  These expressions will be
evaluated by a Lua kernel.  The `<parameters>` element for a Lua load evaluator
contains a single `expressions` element that, in turn, can contain any of the
following parameters:

* `x` -- x-component of the applied force (type: string, default: 0)
* `y` -- y-component of the applied force (type: string, default: 0)
* `z` -- z-component of the applied force (type: string, default: 0)
* `p` -- x-component of the applied moment (type: string, default: 0)
* `q` -- y-component of the applied moment (type: string, default: 0)
* `r` -- z-component of the applied moment (type: string, default: 0)

Each string should conform to the syntax of a Lua expression.  The following
variables can be used inside these expressions:

* `t` -- the current time
* `x` -- the x-coordinate of the point
* `y` -- the y-coordinate of the point
* `z` -- the z-coordinate of the point
* `p` -- the orientation of the point about the x-axis
* `q` -- the orientation of the point about the y-axis
* `r` -- the orientation of the point about the z-axis

The following example applies a force and a moment to a single node in the
structure, according to the expressions:

$$
\begin{aligned}
  f_z &= e^{-t^2} \cos\left(2\pi t + \frac{\pi}{6}\right) \\
  m_x &= \frac{3}{2} y.
\end{aligned}
$$

```xml
<load>
    <id>101</id>
    <type>lua</type>
    <parameters>
        <expressions>
            <z>math.exp(-t * t) * math.cos(2.0 * math.pi * t + math.pi / 6)</z>
            <p>1.5 * y</p>
        </expressions>
    </parameters>
    <elements>
        <element>101</element>
    </elements>
</load>
```

## Solution

This section of the input file is used to define the active degrees of freedom,
the integration method, and the range of integration.  It contains three
elements: `<dofs>`, `<method>`, and `<time>`.

The active degrees of freedom are indicated by providing an appropriate
combination of the following elements within `<dofs>`.

* `x` -- activate translation along the x-coordinate
* `y` -- activate translation along the y-coordinate
* `z` -- activate translation along the z-coordinate
* `p` -- activate rotation about the x-axis
* `q` -- activate rotation about the x-axis
* `r` -- activate rotation about the x-axis

If the `<dofs>` element is missing, then all degrees of freedom are considered
to be active.

The method is defined by two parameters:

* `type` -- integrator type (type: string, default: `newmark_beta`)
* `parameters` -- integrator-specific properties

If the `<type>` element is missing, YAMSS defaults to using the Newmark-$\beta$
method.  Otherwise, it must take one of the following values.

* `newmark_beta` -- Newmark-$\beta$ method
* `generalized_alpha` -- Generalized-$\alpha$ method

More information about these methods is provided below.  The `<time>` element
contains the following properties:

* `span` -- integration interval (type: $\mathbb{R}$, default: 1)
* `step` -- time step (type: $\mathbb{R}$, default: 0.01)

The following input file fragment uses the generalized-$\alpha$ method to
select the Hughes, Hilber, and Taylor $\alpha$ integration method with
$\alpha = -1/3$ ($\alpha = 1 - \alpha_f$).  Only the translational degrees of
freedom are active.  The time interval for integration is set to $t \in [0, 10]$
and the time step is left at its default value of $\Delta t = 0.01$.

```xml
<solution>
    <dofs><x/><y/><z/></dofs>
    <method>
        <type>generalized_alpha</type>
        <parameters>
            <alpha_m>0</alpha_m>
            <alpha_f>0.66667</alpha_f>
        </parameters>
    </method>
    <time>
        <span>10.0</span>
    </time>
</solution>
```

### Newmark-$\beta$ Method

@Newmark-1959-MCS described an integration method for dynamic systems that
uses the extended mean value theorem to express the first and second time
derivatives.  The Newmark-$\beta$ method has two parameters:

* `beta` -- a constant, $\beta$, such that $0\le 2\beta\le 1$
            (type: $\mathbb{R}$, default: 1/4)
* `gamma` -- a constant, $\gamma$, such that $0\le\gamma\le 1$
             (type: $\mathbb{R}$, default: 1/2)

The method introduces artificial damping proportional to $\gamma - 1/2$, so
$\gamma < 1/2$ yields positive damping, $\gamma = 0$ introduces no spurious
damping, and $\gamma > 1/2$ produces negative damping.  The default value
for $\beta = 1/4$ yields the constant average acceleration method.

The Newmark-$\beta$ method is given by

$$
\begin{aligned}
  \left\{\dot{q}\right\}_{n+1} &= \left\{\dot{q}\right\}_n
    + (1 - \gamma)\Delta t\left\{\ddot{q}\right\}_n
    + \gamma\Delta t\left\{\ddot{q}\right\}_{n+1} \\
  \left\{q\right\}_{n+1} &= \left\{q\right\}_n
    + \Delta t\left\{\dot{q}\right\}_n
    + \frac{1}{2}\Delta t^2 \left(
        (1 - 2\beta)\left\{\ddot{q}\right\}_n
        + 2\beta\left\{\ddot{q}\right\}_{n+1}
      \right).
\end{aligned}
$$

$$
\left[M\right]\left\{\ddot{q}\right\}_{n+1}
+ \left[C\right]\left\{\dot{q}\right\}_{n+1}
+ \left[K\right]\left\{\dot{q}\right\}_{n+1}
= \left\{F\right\}_{n+1}
$$

### Generalized-$\alpha$ Method

@Chung-1993-TIA introduces a new family of time integration algorithms for
solving structural dynamics problems.  The new method, denoted as the
generalized-$\alpha$ method, possesses numerical dissipation that can be
controlled by the user.  The present implementation has two free parameters:

* `alpha_m` -- a constant, $\alpha_m$ (type: $\mathbb{R}$, default: 0)
* `alpha_f` -- a constant, $\alpha_f$ (type: $\mathbb{R}$, default: 0)

These constants must be set such that

$$
\alpha_m \le \alpha_f \le \frac{1}{2}
$$

Two other parameters that appear in the method are calculated from $\alpha_m$
and $\alpha_f$ as follows:

$$
\begin{aligned}
  \gamma &= \frac{1}{2} - \alpha_m + \alpha_f \\
  \beta &= \frac{1}{4} \left( 1 - \alpha_m + \alpha_f \right)^2.
\end{aligned}
$$

The basic form of the generalized-$\alpha$ method is given by

$$
\begin{aligned}
  \left\{q\right\}_{n+1} &= \left\{q\right\}_n
    + \Delta t\left\{\dot{q}\right\}_n
    + \frac{1}{2}\Delta t^2\left[
      \left(1 - 2\beta\right)\left\{\ddot{q}\right\}_n
      + 2\beta\left\{\ddot{q}\right\}_{n+1}
      \right] \\
  \left\{\dot{q}\right\}_{n+1} &= \left\{\dot{q}\right\}_n
    + \Delta t\left[
      \left( 1 - \gamma \right) \left\{\ddot{q}\right\}_n
      + \gamma \left\{\ddot{q}\right\}_{n+1}
      \right] \\
\end{aligned}
$$

$$
\left[M\right]\left\{\ddot{q}\right\}_{n+1-\alpha_m}
+ \left[C\right]\left\{\dot{q}\right\}_{n+1-\alpha_f}
+ \left[K\right]\left\{\dot{q}\right\}_{n+1-\alpha_f}
= \left\{F(t_{n+1-\alpha_f})\right\}
$$

where

$$
\begin{aligned}
  \left\{q\right\}_{n+1-\alpha_f} &=
    \left(1 - \alpha_f\right)\left\{q\right\}_{n+1}
    + \alpha_f\left\{q\right\}_n \\
  \left\{\dot{q}\right\}_{n+1-\alpha_f} &=
    \left(1 - \alpha_f\right)\left\{\dot{q}\right\}_{n+1}
    + \alpha_f\left\{\dot{q}\right\}_n \\
  \left\{\ddot{q}\right\}_{n+1-\alpha_m} &=
    \left(1 - \alpha_m\right)\left\{\ddot{q}\right\}_{n+1}
    + \alpha_m\left\{\ddot{q}\right\}_n \\
  t_{n+1-\alpha_f} &= \left(1 - \alpha_f\right) t_{n+1} + \alpha_f t_n.
\end{aligned}
$$

## Outputs

YAMSS does not produce any output unless specifically requested in the
`<outputs>` section of the input file.  Several output filters are available;
each can be activated by including an `<output>` element within `<outputs>`.
Any number of output filters can be active.

The following input file fragment configures YAMSS to generate two types of
output.  The `summary` filter writes summary information to a stream and the
`modes` filter generates a Tecplot data file that contains a history of the
modal coordinates.

```xml
<outputs>
    <output>
        <type>summary</type>
        <parameters>
            <stride>100</stride>
        </parameters>
    </output>
    <output>
        <type>modes</type>
    </output>
</outputs>
```

### Modes Filter

The modes filter outputs an ASCII Tecplot data file containing a history of
the iteration $n$, time $t$, generalized displacements $\left\{q\right\}$,
generalized velocities $\left\{\dot{q}\right\}$, generalized accelerations
$\left\{\ddot{q}\right\}$, and generalized forces $\left\{F\right\}$.
Parameters for the `tecplot` filter are:

* `filename` -- the output file name (type: string, default: `modes.dat`)
* `stride` -- the number of iterations between output
              (type: $\mathbb{N}_1$, default: 1)
* `brief` -- if present, do not include $\left\{\dot{q}\right\}$,
             $\left\{\ddot{q}\right\}$, and $\left\{F\right\}$
* `no_header` -- if present, do not include the Tecplot header lines
                 in the output

If the `filename` is empty, then output is directed to the standard console.

### Motion Filter

The motion filter outputs a series of data files containing snapshots of the
structure.  It's parameters are:

* `filename` -- the output file name template (type: string,
                default `motion/snapshot.%04d.dat`)
* `format` -- the output file format (type: string, default `tecplot`)
* `stride` -- the number of iterations between snapshots
              (type: $\mathbb{N}_1$, default: 1)

The `filename` parameter must contain a single [Boost format][boost_format]
decimal format specification.  This will be replaced by the snapshot number,
starting åfrom zero.

The format can be one of:

* `tecplot` -- ASCII Tecplot data file format (`.dat`)
* `ply` -- Stanford Polygon File Format (`.ply`)

### Property Tree Filter

The property tree filter generates a file containing a history of the
iteration $n$, time $t$, generalized displacements $\left\{q\right\}$,
generalized velocities $\left\{\dot{q}\right\}$, generalized accelerations
$\left\{\ddot{q}\right\}$, and generalized forces $\left\{F\right\}$.  It
accepts the following parameters:

* `filename` -- the output file name (type: string, default: `yamss.xml`)
* `stride` -- the number of iterations between output
              (type: $\mathbb{N}_1$, default: 1)

This filter can write files in XML, JSON, and INFO formats.  The format depends
on the filename extension: `.xml` for XML, `.json` for JSON, and `.info` for
INFO.

### Summary Filter

The summary filter outputs the iteration counter $n$, the time $t$, and a
subset of the modal coordinates $\left\{q\right\}$ as the solution progresses.
It takes the following parameters:

* `filename` -- the output file name (type: string, default: "")
* `stride` -- the number of iterations between output
              (type: $\mathbb{N}_1$, default: 1)
* `limit` -- the maximum number of modes to include in the output
             (type: $\mathbb{N}_1$, default: 3)

If `filename` is empty, then output is directed to the standard console.

# Nomenclature

${\left[\tilde{C}\right]}$
:   Damping matrix.  A symmetric $n\times n$ matrix.

${\left[C\right]}$
:   Modal damping matrix.  A symmetric $m\times m$ matrix.

$\left\{\tilde{F}\right\}$
:   Forcing term.  An $n$-dimensional vector.

$\left\{F\right\}$
:   Modal forcing term.  An $m$-dimensional vector.

${\left[\tilde{K}\right]}$
:   Stiffness matrix.  A symmetric $n\times n$ matrix.

${\left[K\right]}$
:   Modal stiffness matrix.  A symmetric $m\times m$ matrix.

${\left[\tilde{M}\right]}$
:   Mass matrix.  A symmetric $n\times n$ matrix.

${\left[M\right]}$
:   Modal mass matrix.  A symmetric $m\times m$ matrix.

$m$
:   Number of mode shapes.

$n$
:   Number of degrees of freedom.

$p$
:   Number of vertices in an element.

$\left\{q\right\}$
:   Generalized coordinate vector.  An $m$-dimensional vector.

$\left\{x\right\}$
:   Coordinates of the vibrational degrees of freedom.  An $n$-dimensional
    vector.

${\left[\Phi\right]}$
:   Mode shapes.  An $m\times n$ matrix.

${\left[\Psi\right]}$
:   Eigenvectors of the vibrational system.

# References

---
bibliography: references.bib
...

[boost_format]: http://www.boost.org/doc/libs/1_57_0/libs/format
[lua]: http://www.lua.org
