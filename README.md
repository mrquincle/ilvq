<!-- Uses markdown syntax for neat display at github -->

# iLVQ
ILVQ stands for incremental learning vector quantization.

## What does it do?
ILVQ can be used for classification. There are many classification methods. ILVQ belongs to the category of prototype-based classification methods. This means that inputs are represented by prototypical examples. This means that storing all classes can be relatively cheap, only a selected set of instances need to be stored. The way ILVQ works is by using the concept of a winner and a runner-up. Each time the winner wins, it is moved towards the most recent data point, while the runner-up is moved from that data point. 

## Is it good?
Every classification method has its own merits. ILVQ has the following properties: a.) it is incremental (not all training examples need to be there, useful on a robot), b.) the number of clusters do not need to be specific beforehand (as for example with ordinary k-means clustering), and c.) outliers are not removed (no condensing scheme for denoising).

## What are the alternatives?
Almende and DO bots have been using ARTMAP (the unsupervised version of Adaptive Resonance Theory), which is also incremental and also does not need to know the number of clusters in advance. Other alternatives can be: neural gas, and maybe extensions of principle component analysis or k-means clustering. However, because they are not prototype-based and not incremental, this does stretch the imagination.

## An example
Our results for ARTMAP look much better, but the following is the genuine result of ILVQ. There are two classes, one inside a circle, one outside of the circle. The expected result on visualisation would be a perfect circle. However, because of the dynamics in ILVQ it is apparently difficult to create proper circular regions.

![alt text](https://github.com/mrquincle/rur-builder/raw/master/doc/ilvq_circle.jpg "ILVQ circle")

## Where can I read more?
[Wikipedia](http://en.wikipedia.org/wiki/Learning_Vector_Quantization)
* "Rapid Online Learning of Objects in a Biologically Motivated Recognition Architecture" by Kirstein, Wersing, Körner (2005).
* "An incremental learning vector quantization algorithm for pattern classification" by Xu, Shen, Zhao (2010).

## Copyrights
The copyrights (2012) belong to:

- Author: Anne van Rossum
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands
