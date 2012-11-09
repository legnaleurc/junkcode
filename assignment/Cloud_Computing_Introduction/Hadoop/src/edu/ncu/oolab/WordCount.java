package edu.ncu.oolab;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.RecordWriter;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.util.Progressable;

public class WordCount {

	public static class CloudMapper extends MapReduceBase implements Mapper< LongWritable, Text, Text, IntWritable > {
		private final static IntWritable one = new IntWritable( 1 );
		private Text word = new Text();

		@Override
		public void map( LongWritable key, Text value, OutputCollector< Text, IntWritable > output, Reporter reporter ) throws IOException {
			String line = value.toString();
			StringTokenizer tokenizer = new StringTokenizer( line );
			while( tokenizer.hasMoreTokens() ) {
				String token = tokenizer.nextToken();
				if( token.length() < 3 ) {
					continue;
				}
				this.word.set( token );
				output.collect( this.word, one );
			}
		}
	}

	public static class CloudReducer extends MapReduceBase implements Reducer< Text, IntWritable, Text, IntWritable > {
		@Override
		public void reduce( Text key, Iterator< IntWritable > values, OutputCollector< Text, IntWritable > output, Reporter reporter ) throws IOException {
			Integer sum = 0;
			while( values.hasNext() ) {
				sum += values.next().get();
			}
			if( sum < 2 ) {
				return;
			}
			output.collect( key, new IntWritable( sum ) );
		}
	}

	public static class CloudOutputFormat< K, V > extends FileOutputFormat {
		private class CloudRecordWriter< K, V > implements RecordWriter< K, V > {
			private PrintStream out;

			public CloudRecordWriter( OutputStream out ) {
				this.out = new PrintStream( out );
			}

			@Override
			public synchronized void write( K key, V value ) {
				this.out.printf( "%s\t%s\n", value, key );
			}

			@Override
			public synchronized void close( Reporter reporter ) {
				this.out.close();
			}
		}
		@Override
		public RecordWriter< K, V > getRecordWriter( FileSystem ignored, JobConf job, String name, Progressable progress ) {
			FSDataOutputStream stream = null;
			try {
				Path path = FileOutputFormat.getTaskOutputPath( job, name );
				FileSystem fs = path.getFileSystem( job );
				stream = fs.create( path, progress );
			} catch( IOException e ) {
				System.err.println( e.getMessage() );
			}
			return new CloudRecordWriter< K, V >( stream );
		}
	}

	public static void main( String[] args ) {
		JobConf conf = new JobConf( WordCount.class );
		conf.setOutputKeyClass( Text.class );
		conf.setOutputValueClass( IntWritable.class );
		conf.setMapperClass( CloudMapper.class );
		conf.setCombinerClass( CloudReducer.class );
		conf.setReducerClass( CloudReducer.class );
		conf.setInputFormat( TextInputFormat.class );
		conf.setOutputFormat( CloudOutputFormat.class );
		FileInputFormat.setInputPaths( conf, new Path( args[0] ) );
		FileOutputFormat.setOutputPath( conf, new Path( args[1] ) );
		try {
			JobClient.runJob( conf );
		} catch( Exception e ) {
			System.err.println( e.getMessage() );
		}
	}

}
