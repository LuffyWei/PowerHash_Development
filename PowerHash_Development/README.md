# PowerHash_Development
the development of powerhash
 通过1.0版本认识了bHash,也就是indexing-filling方法。
 通过2.0版本引入count-min sketch对group size进行粗略统计
 通过3.0版本引入big group和small group的单独处理的思想，但是只是处理big group，small group不作处理
 通过4.0版本引入key_file的二进制块读出和写入的工程思想。
 通过5.0版本引入：还是通过精确group size生成输出索引更合适的想法，key_file记录<key, valuesize>。 同时，引入将small group统一写入accumulate单元，但是仍然不作处理。
 通过6.0版本引入：将accumulate单元分区的思想，也就是对small groups进行分区的思想，但是small group仍然不作处理。
 通过7.3版本引入适用于所有数据集的key_file二进制读写方法，即key结构体设置为<char[], int>类型。
 通过8.0版本正是确定算法的思想：大组和小组分开处理的核心思想。
通过9.0-11.2版本进行参数设置改进：
 通过9.0版本的参数设置，虽然方法有漏洞，但是可以确定index offset大小的边界值，即确定最多能够容纳多少个big group的输出索引。
 通过11.0版本，将一直沿用的将原始文件进行spill分区的小技巧,减少了参数的设置，而且计算出的big group num可以直接作为参数，不需要再认为设定。
 通过11.1版本，将big groups和small group分开放，免得还要记录每个small group partition的offset.
 通过11.2版本，提出人为设置big group和small group的比例，同时发现pareto principle.
最终获得12.0版本，即powerhash:
powerhash思想和8.0一样，不过调参方式是参照pareto principle定律。
