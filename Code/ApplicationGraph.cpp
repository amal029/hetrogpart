// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 20.08.2012

/** @file ApplicationGraph.cpp
 *
 */

#ifndef APPLICATION_GRAPH_CPP
#define APPLICATION_GRAPH_CPP

#include "ApplicationGraph.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

//ApplicationGraph()
ApplicationGraph :: ApplicationGraph()
{
	no_of_constraints = 0;
	no_of_vertices = 0;
	no_of_edges = 0;
}
//end of ApplicationGraph()

//ReadGraph( char* file_name )
int ApplicationGraph :: ReadGraph( string file_name )
{
	ifstream in( file_name.c_str() );

	dynamic_properties dp;

	dp.property( "name", get( &AppNode::name, app_graph ) );
	dp.property( "label", get( &AppNode::label, app_graph ) );
	dp.property( "type", get( &AppNode::type, app_graph ) );
	dp.property( "constraints", get( &AppNode::constraints, app_graph ) );
	dp.property( "shape", get( &AppNode::shape, app_graph ) );
	dp.property( "weight", get( &AppEdge::weight, app_graph ) );
	dp.property( "label", get( &AppEdge::label, app_graph ) );
	dp.property( "Id", get( &AppNode::id, app_graph ) );

	bool status = read_graphviz( in, app_graph, dp, "name" );
	in.close();

	no_of_vertices = num_vertices( app_graph );
	no_of_edges = num_edges( app_graph );

	ParseConstraints();
	SetApplicationParameters();

	PrintGraphViz( "app_graph.dot" );

	return status;
}
//end of ReadGraph( char* file_name )

int32_t ApplicationGraph :: ReadMetisFormat( string file_name )
{
	const string& delimiters = " ";
	ifstream input_file;
	input_file.open( file_name.c_str() );

	graph_traits< ApplicationGraphType >::vertex_descriptor v;
	graph_traits< ApplicationGraphType >::vertex_iterator v_iter, v_end;

	uint32_t line_no = 1;
	while( input_file.good() )
	{
		string line;
		vector< double_t > tokens;
		getline( input_file, line );

		// Skip delimiters at beginning.
		string::size_type lastPos = line.find_first_not_of( delimiters, 0 );

		// Find first "non-delimiter".
		string::size_type pos = line.find_first_of( delimiters, lastPos );

		while( string::npos != pos || string::npos != lastPos )
		{
			// Found a token, add it to the vector.
			tokens.push_back( lexical_cast< uint64_t > ( line.substr( lastPos, pos - lastPos ) ) );

			// Skip delimiters.  Note the "not_of"
			lastPos = line.find_first_not_of( delimiters, pos );

			// Find next "non-delimiter"
			pos = line.find_first_of( delimiters, lastPos );
		}
		/*
		for( uint32_t i = 0; i < tokens.size(); i++ )
		{
			cout << tokens[ i ] << " ";
		}
		cout << endl;
		*/

		if( tokens.size() == 0 )
			continue;

		if( line_no == 1 )
		{
			if( tokens.size() != 4 )
			{
				cout << "ERROR: Input graph file format incorrect" << endl;
				return -1;
			}

			if( tokens[ 2 ] != 11 )
			{
				cout << "ERROR: Input graph without constraints specified" << endl;
				return -1;
			}

			no_of_vertices = tokens[ 0 ];
			no_of_edges = tokens[ 1 ];
			no_of_constraints = tokens[ 3 ];

			app_graph = ApplicationGraphType( no_of_vertices );
			tie( v_iter, v_end ) = vertices( app_graph );

			uint32_t index = 1;
			BGL_FORALL_VERTICES( v1, app_graph, ApplicationGraphType )
			{
				app_graph[ v1 ].id = index;
				app_graph[ v1 ].d_id = index;
				index++;
			}

			line_no++;
			continue;
		}

		if( ( tokens.size() - no_of_constraints ) % 2 != 0 )
		{
			cout << "ERROR: Missing edge pair in line no." << line_no << endl;
			return -1;
		}

		v = *v_iter;

		string label = "Id=" + lexical_cast< string >( app_graph[ v ].id ) + " Cnstr=";

		for( uint32_t i = 0; i < no_of_constraints; i++ )
		{
			app_graph[ v ].constraint.push_back( tokens[ i ] );
			label += lexical_cast< string >( tokens[ i ] ) +  " ";
		}
		app_graph[ v ].label = label;
		app_graph[ v ].shape = "square";

		//cout << label << endl;

		for( uint32_t i = no_of_constraints; i < tokens.size(); i+=2 )
		{
			bool found = false;
			graph_traits< ApplicationGraphType >::out_edge_iterator out_i, out_end;
			graph_traits< ApplicationGraphType >::edge_descriptor e;
			for( tie( out_i, out_end ) = out_edges( v, app_graph ); out_i != out_end; ++out_i )
			{
				e = *out_i; //assign edge desciptor to iterator
				//cout << app_graph[ source( e, app_graph ) ].id << " -> ";
				//cout << app_graph[ target( e, app_graph ) ].id;
				//cout << "  " << app_graph[ e ].weight << endl;

				if( app_graph[ target( e, app_graph ) ].id == tokens[ i ] )
				{
					if( app_graph[ e ].weight != tokens[ i + 1 ] )
					{
						cout << "ERROR: Incorrect weight for edge " ;
						cout << app_graph[ source( e, app_graph ) ].id << " -> ";
						cout << app_graph[ target( e, app_graph ) ].id;
						cout << ", Found " << app_graph[ e ].weight << " Instead of ";
						cout << tokens[ i + 1 ] << " @D=" << tokens[ i ] << endl;
					}
					found = true;
				}
			}

			if( found != true )
			{
				std::pair< graph_traits< ApplicationGraphType >::edge_descriptor, bool > edge_pair;
				edge_pair = add_edge( ( app_graph[ v ].id - 1 ), ( tokens[ i ] - 1 ), app_graph );
				//cout << "\t\tAdd edge " << app_graph[ v ].id << " " << ( app_graph[ v ].id - 1 ) << " to " << tokens[ i ] << " " << ( tokens[ i ] - 1 ) << " with " << tokens[ i + 1 ] << endl;
				app_graph[ edge_pair.first ].weight = tokens[ i + 1 ];
				//cout << "\t\tAdded edge " << app_graph[ source( edge_pair.first, app_graph ) ].id << " -> " << app_graph[ target( edge_pair.first, app_graph ) ].id << " " << app_graph[ edge_pair.first ].weight << endl;
			}
		}

		line_no++;
		v_iter++;

		if( v_iter == v_end )
		{
			//cout << "ERROR: No of vertices do not match that specified" << endl;
			//cout << "Line no: " << line_no << endl;
			break;
		}
	}

	SetApplicationParameters();
	PrintGraphViz( "app_graph.dot" );

	return 1;
}

//ParseConstraints()
void ApplicationGraph :: ParseConstraints()
{
	const string& delimiters = ",";

	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = app_graph[ v ].constraints.find_first_not_of( delimiters, 0 );

		// Find first "non-delimiter".
		string::size_type pos = app_graph[ v ].constraints.find_first_of( delimiters, lastPos );

		while( string::npos != pos || string::npos != lastPos )
		{
			// Found a token, add it to the vector.
			app_graph[ v ].constraint.push_back(  lexical_cast< float_t > ( app_graph[ v ].constraints.substr( lastPos, pos - lastPos ) ) );

			// Skip delimiters.  Note the "not_of"
			lastPos = app_graph[ v ].constraints.find_first_not_of( delimiters, pos );

			// Find next "non-delimiter"
			pos = app_graph[ v ].constraints.find_first_of( delimiters, lastPos );
		}

		if( app_graph[ v ].constraint.size() > no_of_constraints )
			no_of_constraints = app_graph[ v ].constraint.size();
	}
}
//end of ParseConstraints()

//SetApplicationParameters()
void ApplicationGraph :: SetApplicationParameters()
{
	//set constraint mask to 1 for the nmber of constraints
	constraint_mask.resize( no_of_constraints, 1 );

	/*
	 * Provide uniform no of constraints for all nodes in the app graph
	 * If no of constraints is less then pad the rest with 0
	 * calculate the sum of each constraints
	 */
	const_wgt_tot.resize( no_of_constraints, 0 );
	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		if( app_graph[ v ].constraint.size() < no_of_constraints )
		{
			for( uint32_t i = 0; i < no_of_constraints; i++ )
				app_graph[ v ].constraint.push_back( 0 );
		}

		for( uint32_t i = 0; i < no_of_constraints; i++ )
		{
			const_wgt_tot[ i ] += app_graph[ v ].constraint[ i ];
		}
	}

	/*
	 * Strip the columns for constraints which are zero
	 */
	for( uint32_t i = 0; i < no_of_constraints; i++ )
	{
		if( const_wgt_tot[ i ] == 0 )
		{
			no_of_constraints--;
			constraint_mask[ i ] = 0;

			BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
			{
				app_graph[ v ].constraint.erase( app_graph[ v ].constraint.begin() + i );
			}
		}
	}

	BGL_FORALL_EDGES( e, app_graph, ApplicationGraphType )
	{
		app_graph[ e ].label = lexical_cast< string >( app_graph[ e ].weight );
	}

	if( no_of_vertices > 0 )
	{
		//display some info about application
		cout << "INFO: App: " << "V=" << no_of_vertices;
		cout << "\tE=" << no_of_edges << "\tConstraint Tot: ";
		for( uint32_t i = 0; i < no_of_constraints; i++ )
		{
			cout << const_wgt_tot[ i ] << " ";
		}
		cout << "\tConstraint Mask: ";
		for( uint32_t i = 0; i < no_of_constraints; i++ )
		{
			cout << constraint_mask[ i ] << " ";
		}
		cout << endl;
	}

	/*
	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		cout << app_graph[ v ].id << ":";
		for( uint32_t i = 0; i < app_graph[ v ].constraint.size(); i++ )
			cout << ";" << app_graph[ v ].constraint[ i ];
		cout << endl;
	}
	*/
}
//end of SetApplicationParameters()

//PrintGraphViz( const char* output_file )
int32_t ApplicationGraph :: PrintGraphViz( string output_file )
{
	dynamic_properties dp;

	dp.property( "name", get( &AppNode::name, app_graph ) );
	dp.property( "label", get( &AppNode::label, app_graph ) );
	dp.property( "type", get( &AppNode::type, app_graph ) );
	dp.property( "constraints", get( &AppNode::constraints, app_graph ) );
	dp.property( "shape", get( &AppNode::shape, app_graph ) );
	dp.property( "weight", get( &AppEdge::weight, app_graph ) );
	dp.property( "label", get( &AppEdge::label, app_graph ) );
	dp.property( "id", get( &AppNode::id, app_graph ) );

	ofstream out( output_file.c_str() );
	write_graphviz_dp( out, app_graph, dp, string(), get( &AppNode::id, app_graph ) );
	out.close();

	return 0;
}
//end of PrintGraphViz( const char* output_file )

int32_t ApplicationGraph :: GenerateAppSubGraphs( vector< vector< uint32_t > > *partitions,
											vector< ApplicationGraph* > *app_graph_vector )
{
	if( partitions->size() == 0 )
	{
		cout << "ERROR: Application Partition size requested is not possible " << partitions->size() << endl;
		return -1;
	}

	for( uint32_t i = 0; i < partitions->size(); i++ )
	{
		ApplicationGraph *app_graph_obj = new ApplicationGraph();
		app_graph_obj->app_graph = ApplicationGraphType( partitions->at( i ).size() );

		//cout << "ids: ";
		uint32_t j = 0;
		BGL_FORALL_VERTICES( v_sub, app_graph_obj->app_graph, ApplicationGraphType )
		{
			app_graph_obj->app_graph[ v_sub ].id = partitions->at( i ).at( j );
			app_graph_obj->app_graph[ v_sub ].d_id = j + 1;
			//cout << partitions->at( i ).at( j ) << " ";
			j++;
		}
		//cout << endl;

		BGL_FORALL_VERTICES( v_sub, app_graph_obj->app_graph, ApplicationGraphType )
		{
			BGL_FORALL_VERTICES( v_orig, app_graph, ApplicationGraphType )
			{
				if( app_graph[ v_orig ].id == app_graph_obj->app_graph[ v_sub ].id )
				{
					app_graph_obj->app_graph[ v_sub ].label = app_graph[ v_orig ].label;
					app_graph_obj->app_graph[ v_sub ].shape = app_graph[ v_orig ].shape;
					app_graph_obj->app_graph[ v_sub ].constraint = app_graph[ v_orig ].constraint;
					break;
				}
			}
		}

		//BGL_FORALL_VERTICES( v_ov, app_graph_obj->app_graph, ApplicationGraphType )
		//{
			//BGL_FORALL_VERTICES( v_orig, app_graph, ApplicationGraphType )
			//{
				//BGL_FORALL_OUTEDGES( v_orig, e, app_graph, ApplicationGraphType )
				BGL_FORALL_EDGES( e, app_graph, ApplicationGraphType )
				{
					graph_traits< ApplicationGraphType >::vertex_descriptor sub_v_s = 0, sub_v_d = 0;
					bool found_s = false, found_d = false;
					BGL_FORALL_VERTICES( v_sub, app_graph_obj->app_graph, ApplicationGraphType )
					{
						if( app_graph[ source( e, app_graph ) ].id == app_graph_obj->app_graph[ v_sub ].id )
						{
							found_s = true;
							sub_v_s = v_sub;
						}

						if( app_graph[ target( e, app_graph ) ].id == app_graph_obj->app_graph[ v_sub ].id )
						{
							found_d = true;
							sub_v_d = v_sub;
						}

						if( found_s && found_d )
							break;
					}

					bool found = false;
					if( found_s && found_d )
					{
						BGL_FORALL_EDGES( s_e, app_graph_obj->app_graph, ApplicationGraphType )
						{
							if( ( app_graph[ source( e, app_graph ) ].id == app_graph_obj->app_graph[ source( s_e, app_graph_obj->app_graph ) ].id &&
									app_graph[ target( e, app_graph ) ].id == app_graph_obj->app_graph[ target( s_e, app_graph_obj->app_graph ) ].id ) ||
								( app_graph[ source( e, app_graph ) ].id == app_graph_obj->app_graph[ target( s_e, app_graph_obj->app_graph ) ].id &&
									app_graph[ target( e, app_graph ) ].id == app_graph_obj->app_graph[ source( s_e, app_graph_obj->app_graph ) ].id ) )
							{
								found = true;
								break;
							}
						}
					}
					//else
					//{
					//	break;
					//}

					//if( found )
					//{
					//	break;
					//}
					//else
					if( !found && found_s && found_d )
					{
						std::pair< graph_traits< ApplicationGraphType >::edge_descriptor, bool > edge_pair;
						edge_pair = add_edge( sub_v_s, sub_v_d, app_graph_obj->app_graph );
						app_graph_obj->app_graph[ edge_pair.first ].weight = app_graph[ e ].weight;
						//cout << "\t\tSubgraph Added edge " << app_graph_obj->app_graph[ source( edge_pair.first, app_graph_obj->app_graph ) ].id << " -> ";
						//cout << app_graph_obj->app_graph[ target( edge_pair.first, app_graph_obj->app_graph ) ].id << " ";
						//cout << app_graph_obj->app_graph[ edge_pair.first ].weight << endl;
					}
				}
			//}
		//}

		app_graph_obj->no_of_vertices = num_vertices( app_graph_obj->app_graph );
		app_graph_obj->no_of_edges = num_edges( app_graph_obj->app_graph );
		app_graph_obj->no_of_constraints = no_of_constraints;

		if( app_graph_obj->no_of_vertices > 0 )
			cout << "INFO: Subgraph : " << i << " ";
		app_graph_obj->SetApplicationParameters();
		app_graph_vector->push_back( app_graph_obj );
	}
	return 1;
}

int32_t ApplicationGraph :: SetTpWgts( vector< vector< float_t >* > *tp_wgts, vector< int32_t > *part_ids,
										vector< int32_t > *wgt_idx, vector< float_t > *part_sizes )
{
	/*
	if( tp_wgts->at( 0 )->size() != constraint_mask.size() )
	{
		cout << "WARNING: No of constraints of application and topology do not match" << endl;
		//return -1;
	}
	*/

	for( uint32_t i = 0; i < tp_wgts->size(); i++ )
	{
		for( uint32_t w_index = 0, j = 0; j < tp_wgts->at( i )->size() && j < constraint_mask.size(); j++ )
		{
			if( constraint_mask.at( j ) == 1 )
			{
				part_ids->push_back( i );
				wgt_idx->push_back( w_index++ );
				part_sizes->push_back( tp_wgts->at( i )->at( j ) );
			}
		}
	}

	return 1;
}

int32_t ApplicationGraph :: SetTpWgts( vector< vector< float_t >* > *tp_wgts, vector< real_t > *tpwgts )
{
	/*
	if( tp_wgts->at( 0 )->size() != constraint_mask.size() )
	{
		cout << "WARNING: No of constraints of application and topology do not match" << endl;
		//return -1;
	}
	*/

	for( uint32_t i = 0; i < tp_wgts->size(); i++ )
	{
		for( uint32_t j = 0; j < tp_wgts->at( i )->size() && j < constraint_mask.size(); j++ )
		{
			if( constraint_mask.at( j ) == 1 )
			{
				tpwgts->push_back( lexical_cast< real_t >( tp_wgts->at( i )->at( j ) ) );
			}
		}
	}

	return 1;
}

bool ApplicationGraph :: ReadApplication( string filename )
{
	cout << endl << "INFO: Reading Application Graph " << filename << endl;

	if( filename.find( ".dot", filename.length() - 4 ) != string::npos )
	{
		bool status = ReadGraph( filename.c_str() );
		if( status )
		{
			cout << "INFO: Successfully read application in dot format" << endl;
			return true;
		}
	}
	else
	{
		int32_t status = ReadMetisFormat( filename.c_str() );
		if( status )
		{
			cout << "INFO: Successfully read application in metis format" << endl;
			return true;
		}
		else
		{
			cout << "ERROR: " << status << " in reading application from metis format" << endl;
		}
	}
	return false;
}

void ApplicationGraph :: GenerateMetisFile( string filename )
{
	ofstream output_file;
	output_file.open( filename.c_str() );

	output_file << no_of_vertices << " " << no_of_edges << " 011 " << no_of_constraints << endl;

	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		for( uint32_t i = 0; i < no_of_constraints; i++ )
			output_file << lexical_cast< unsigned long >( app_graph[ v ].constraint[ i ] ) << " ";

		BGL_FORALL_OUTEDGES( v, e, app_graph, ApplicationGraphType )
		{
			output_file.setf(ios::fixed, ios::floatfield);
			output_file << ( app_graph[ target( e, app_graph ) ].d_id + 0 ) << " " << ( uint32_t ) app_graph[ e ].weight << " ";
		}

		output_file << endl;
	}
}

void ApplicationGraph :: GenerateMetisFileAddDummyNode( string filename )
{
	ofstream output_file;
	output_file.open( filename.c_str() );

	output_file << no_of_vertices + 2 << " " << no_of_edges + 1 << " 011 " << no_of_constraints << endl;

	uint32_t m_id = 1;
	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		for( uint32_t i = 0; i < no_of_constraints; i++ )
			output_file << lexical_cast< unsigned long >( app_graph[ v ].constraint[ i ] ) << " ";

		BGL_FORALL_OUTEDGES( v, e, app_graph, ApplicationGraphType )
		{
			output_file.setf(ios::fixed, ios::floatfield);
			output_file << ( app_graph[ target( e, app_graph ) ].d_id + 0 ) << " " << ( uint32_t ) app_graph[ e ].weight << " ";
		}

		output_file << endl;
		m_id = app_graph[ v ].d_id;
	}

	for( uint32_t i = 0; i < no_of_constraints; i++ )
			output_file << 0 << " ";

	output_file << "1 " << m_id + 1 << endl;


	for( uint32_t i = 0; i < no_of_constraints; i++ )
			output_file << 0 << " ";

	output_file << "1 " << m_id << endl;
}

#endif
