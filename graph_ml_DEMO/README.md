# Graph Machine Learning Demo

In the Machine-Learning Demo the High-Performance Data and Graph Analytic course you will have the chance to play with **link prediction**, and with state-of-the-art **graph machine learning** models.

Link prediction means that, given two vertices from our input graph, we want to predict whether a link (or edge) should exist between the two vertices. For example, you might want to predict whether two users of a social network should become friends! Or if a user should buy a certain product, starting from a bipartite user-product purchases graph. In this competition, you will work with a list of 24 graphs that represent **protein-protein interactions (PPI)**. Each graph represents a human tissue, and vertices are proteins. Links are protein-protein interactions, and your goal is to predict whether two proteins interact. The [Dataset](#dataset) section gives additional details about the data employed in this context.

While there has been a lot of interesting research in predicting proteins' structure and interactions with Deep Learning, what we really care about is the ability to create powerful embeddings for each element in these graphs. You will study how to transform the information contained in a vertex (either considering just the graph topology or all its features) to a low-dimensional vector, which can then be fed to a classifier to obtain predictions about these.

## Graph Representation Learning

The study of vertex embeddings has really exploded in the recent past, thanks to the discoveries in the field of Deep Learning and to the abundance of graph data to play with (Google, Facebook, Amazon, ..., you can really make a long list here!). In the [Documents](#documents) section you'll find a list of very interesting papers on the topic, that you can use as a starting point for your experiments.

The `ppi-link_prediction-example.ipynb` notebook offers an example of link prediction application on the PPI dataset, implemented through the [StellarGraph](https://stellargraph.readthedocs.io/en/stable/README.html) library. The notebook also provides useful code for data handling and model evaluation. 

We start our analysis using the famous GCN model (Graph Convolutional Network), showing you a simple model that can already perform quite well. 

Your job is to improve this result, either by playing with GCN parameters, by finding a better embedding algorithm, or by creating one yourself (don't put limits to your ambitions!).

## Dataset
The PPI dataset you'll use in this competition is stored in the `input` folder, and it is made of 3 files:
- `ppi_v.csv` is the list of vertices, with the features of each vertex. Please **disregard** the **dataset** column, it is not used in this context
* `ppi_e_train.csv` is the list of edges/links available for training. All these edges are real!
* `ppi_e_test.csv` is the list of edges for which you have to make a prediction. Some are real, some aren't!

## Evaluation

The evaluation metric for this demo is [Mean F1-Score](https://en.wikipedia.org/wiki/F-score). The F1 score is an accuracy metric computed as harmonic average of precision **P** and recall **R**. Precision is the ratio of true positives (**TP**) to all predicted positives (**TP** + **FP** ). Recall is the ratio of true positives to all actual positives (**TP** + **FN**).

The F1 score is given by:

<img src="https://render.githubusercontent.com/render/math?math=F1 = 2\frac{P \cdot R}{P%2BR}\ \ \mathrm{where}\ \ P = \frac{TP}{TP%2BFP},\ \ R = \frac{TP}{TP%2BFN}">

The F1 metric weights recall and precision equally, and a good link prediction algorithm will maximize both precision and recall simultaneously. Thus, moderately good performance on both will be favored over extremely good performance on one and poor performance on the other.

### Output Format

**For every link in the test dataset**, the output files should contain two columns: **id** and **label**. The **id** is built as **sourceidx + _ + destinationidx**. The sample notebook already shows how to do this! Also, make sure that there are no duplicate predictions. The `sample_ppi_predictions.csv` file has been created with our sample notebook. It shows you what the output file should look like.

The file must contain the header and have the following format:

``` 
id,label 
29842_56942,0 
19107_10249,0 
53567_20998,1 
51834_54178,0 
8316_8730,1 
29315_51587,1 ... 
```

## Important Remarks

The main goal of this demo is not to obtain a super amazing prediction accuracy (even though that's also nice to have), but to show that you have a solid grasp of how graph machine learning works and that you can make good use of it.

The same applies to feature engineering or other things you might be used to doing if you have any experience with data science. Don't spend time hand-crafting features or looking online for more features for your proteins! Get in touch with us if you have doubts or questions :)

Another thing to keep in mind is that you can easily find online pre-made models that will give you great results, and that is basically the state-of-the-art in this field. Some of them are even linked in the papers we have provided! You can use existing algorithms or even existing code, but you should be well aware of what you are doing. Try different algorithms and spend time fine-tuning existing models or making incremental changes to them, and focus on learning what you are doing.

## Documents

This is a collection of old-but-gold papers on graph machine learning. Feel free to read them to improve your knowledge on the topic, and to find new sources of ideas for the competition.

* [Graph embeddings survey][1]: a survey that compares most of the previous approaches in a very concise and understandable way. It uses a unified **encoder-decoder** framework to represent most embeddings algorithms.  A great starting point for your research!
* [DeepWalk][2]: one of the first vertex embeddings algorithms, it works by basically applying word2vec (an algorithm that creates embeddings of words) on random walks on a graph, as vertices in the walk can be seen as words in a sentence. 
* [Blog on Graph Convolutions][3]: a very nice blog post about Graph Convolutional Networks, an essential component of most recent graph embeddings algorithms. Read this before jumping into more complex papers!
* [Graph Attention Networks][4]: an approach that uses attention to assign different importance to nodes in the same neighborhood. It achieves state-of-the-art results on the PPI dataset. Also presented in [this][5] blog post, if you want a simpler explanation.
* [Geometric Deep Learning][6]: a nice journal article about applying deep learning to non-euclidean domains, such as graphs and 3D manifolds. A very interesting read if you want to learn more on the topic.
* [Watch Your Step][7]: another paper about attention applied to graph embeddings, it also achieves state-of-the-art results on the PPI dataset.
* [Another survey][8]: a comprehensive survey on graph embeddings, a great starting point to learn about the topic.
* [GraphSage][9]: an embedding algorithm that learns how to aggregate the neighborhood of a vertex, and was used (with small changes) by [Pinterest][10] on a large graph with billions of vertices.
* [LINE][11] and [node2vec][12]: slightly older embeddings algorithms, used very often as baselines. Give them a try before using more recent (and complex) approaches.
* [One more survey][13]: a survey on Graph Neural Networks, it presents a comprehensive taxonomy of all graph neural networks presented so far in the literature.

  [1]: https://arxiv.org/pdf/1709.05584.pdf
  [2]: http://www.perozzi.net/publications/14_kdd_deepwalk.pdf
  [3]: https://tkipf.github.io/graph-convolutional-networks/
  [4]: https://arxiv.org/pdf/1710.10903.pdf
  [5]: http://petar-v.com/GAT/
  [6]: https://arxiv.org/pdf/1611.08097.pdf
  [7]: https://arxiv.org/pdf/1710.09599.pdf
  [8]: https://arxiv.org/pdf/1709.07604.pdf
  [9]: https://arxiv.org/pdf/1706.02216.pdf
  [10]: https://arxiv.org/pdf/1806.01973.pdf
  [11]: http://www.www2015.it/documents/proceedings/proceedings/p1067.pdf
  [12]: https://cs.stanford.edu/~jure/pubs/node2vec-kdd16.pdf
  [13]: https://arxiv.org/pdf/1812.08434.pdf