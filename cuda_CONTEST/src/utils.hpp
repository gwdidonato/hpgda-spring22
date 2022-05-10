#pragma once

#include <vector>
#include <tuple>
#include <algorithm>
#include <map>
#include "mmio.hpp"
#include "dvrapi_error_string.h"

#define checkCudaErrors(err) __checkCudaErrors(err, __FILE__, __LINE__)

// These are the inline versions for all of the SDK helper functions
inline void __checkCudaErrors(int err, const char *file, const int line) {
  if (0 != err) {
    fprintf(stderr,
            "checkCudaErrors() Driver API error = %04d \"%s\" from file <%s>, "
            "line %i.\n",
            err, getCudaDrvErrorString(err), file, line);
    exit(EXIT_FAILURE);
  }
}

template<typename T> struct map_init_helper
{
    T& data;
    map_init_helper(T& d) : data(d) {}
    map_init_helper& operator() (typename T::key_type const& key, typename T::mapped_type const& value)
    {
        data[key] = value;
        return *this;
    }
};

template<typename T> map_init_helper<T> map_init(T& item)
{
    return map_init_helper<T>(item);
}

//////////////////////////////
// Print functions ///////////
//////////////////////////////

template<typename T>
inline std::string format_array(T *v, int n, uint max = 20) {
	int max_val = std::min(n, (int) max);
	std::string out = "[";
	for (int i = 0; i < max_val; i++) {
		out += std::to_string(v[i]);
		if (i < max_val - 1) {
			out += ", ";
		}
	}
	return out + "]";
}

template<typename T>
inline void print_array(T *v, int n, uint max = 20) {
	std::cout << format_array(v, n, max) << std::endl;
}

template<typename T>
inline void print_array(std::vector<T> &v, uint max = 20) {
	print_array(v.data(), v.size(), max);
}

template<typename T>
inline void print_array_indexed(T *v, uint n, uint max = 20) {
	uint start1 = 0;
	uint start2 = 0;
	uint end1 = n;
	uint end2 = n;

	if (n > max) {
		end1 = max / 2;
		start2 = n - max / 2;
	}
	for (uint i = start1; i < end1; i++) {
		std::cout << i << ") " << v[i] << std::endl;
	}
	if (n > max) {
		std::cout << "[...]" << std::endl;
		for (uint i = start2; i < end2; i++) {
			std::cout << i << ") " << v[i] << std::endl;
		}
	}
	std::cout << std::endl;
}

template<typename T>
inline void print_array_indexed(std::vector<T> &v, uint max = 20) {
	print_array_indexed(v.data(), v.size(), max);
}


//////////////////////////////
// Read MTX file /////////////
//////////////////////////////

// Utility functions adapted from Graphblast, used to read MTX files;

template<typename I, typename T>
inline void read_tuples(std::vector<I> *row_indices, std::vector<I> *col_indices, std::vector<T> *values,
        I nvals, FILE *f,
		bool read_tuples_transposed = false,
		bool read_value_column = false,
		bool zero_indexed_file = false,
		bool debug = false) {
	I row_ind, col_ind;
	I row_ind_i, col_ind_i;
	double value = 1;

	// Buffer to store the first line we read from the file;
	int max_line_size = 400;
	char* first_line_buffer = (char*) malloc(max_line_size);
	bool file_has_two_cols = false;

	// Currently checks if there are fewer rows than promised
	// Could add check for edges in diagonal of adjacency matrix
	int err = 0;
	for (I i = 0; i < nvals; i++) {
		if (i > 0) {
			if (err = fscanf(f, "%u", &row_ind_i) == EOF) {
				std::cout << "Error: Not enough rows in mtx file!\n";
				return;
			} else {
				err = fscanf(f, "%u", &col_ind_i);
				// Read the third column if necessary;
				if (!file_has_two_cols) {
					err = fscanf(f, "%lf", &value);
				}
			}
		} else {
			char* err = fgets(first_line_buffer, max_line_size, f);
			int space_count = 0;
			for (int j = 1; j < max_line_size && first_line_buffer[j] != '\n'; j++) {
				if (first_line_buffer[j] == ' ' && first_line_buffer[j - 1] != ' ') space_count++;
			}
			if (space_count == 1) file_has_two_cols = true;
			else if (space_count == 2) file_has_two_cols = false;
			else {
				std::cout << "Invalid number of columns in file: " << space_count + 1 << std::endl;
				return;
			}
			if (file_has_two_cols) sscanf(first_line_buffer, "%u %u\n", &row_ind_i, &col_ind_i);
			else sscanf(first_line_buffer, "%u %u %lf\n", &row_ind_i, &col_ind_i, &value);
			if (debug && err == nullptr) std::cout << "error in fgets" << std::endl;
		}
		// If we don't care about the value column, overwrite it;
		if (!read_value_column) {
			value = 1;
		}
		row_ind = (I) row_ind_i;
		col_ind = (I) col_ind_i;

		// Convert 1-based indexing MTX to 0-based indexing C++
		if (!zero_indexed_file) {
			row_ind--;
			col_ind--;
		}
		if (read_tuples_transposed) {
			row_indices->push_back(col_ind);
			col_indices->push_back(row_ind);
		} else {
			row_indices->push_back(row_ind);
			col_indices->push_back(col_ind);
		}
		values->push_back(value);
	}
	if (debug) std::cout << "scanf output=" << err << std::endl;
	free(first_line_buffer);
}

template<typename I, typename T>
inline bool compare(const std::tuple<I, I, T, I> &lhs,
		const std::tuple<I, I, T, I> &rhs) {
	I a = std::get<0>(lhs);
	I b = std::get<0>(rhs);
	I c = std::get<1>(lhs);
	I d = std::get<1>(rhs);
	if (a == b)
		return c < d;
	else
		return a < b;
}

template<typename I>
inline bool compare(const std::tuple<I, I, I> &lhs,
		const std::tuple<I, I, I> &rhs) {
	I a = std::get<0>(lhs);
	I b = std::get<0>(rhs);
	I c = std::get<1>(lhs);
	I d = std::get<1>(rhs);
	if (a == b)
		return c < d;
	else
		return a < b;
}

template<typename I, typename T>
inline void custom_sort(std::vector<I> *row_indices, std::vector<I> *col_indices, std::vector<T> *values) {
	I nvals = row_indices->size();
	std::vector<std::tuple<I, I, T, I>> my_tuple;

	for (I i = 0; i < nvals; ++i)
		my_tuple.push_back(std::make_tuple((*row_indices)[i], (*col_indices)[i], (*values)[i], i));

	std::sort(my_tuple.begin(), my_tuple.end(), compare<I, T>);

	std::vector<I> v1 = *row_indices;
	std::vector<I> v2 = *col_indices;
	std::vector<T> v3 = *values;

	for (I i = 0; i < nvals; ++i) {
		I index = std::get<3>(my_tuple[i]);
		(*row_indices)[i] = v1[index];
		(*col_indices)[i] = v2[index];
		(*values)[i] = v3[index];
	}
}

template<typename I, typename T>
inline int read_mtx(const char *fname,
		std::vector<I> *row_indices, std::vector<I> *col_indices, std::vector<T> *values,
		I* num_rows, I* num_cols, I* num_nnz,
		bool read_tuples_transposed = false, // If true, read edges transposed, i.e. edge (2, 3) is loaded as (3, 2);
		bool read_value_column = false,      // If true, read the third column of the matrix file. If false, set all values to 1 (this is what you want when reading a graph topology);
		bool debug = false,                  // If true, print additional information;
		bool zero_indexed_file = false,      // MTX files use indices starting from 1. If for whatever reason your MTX files uses indices that start from 0, set zero_indexed_file=true;
		bool sort_tuples = false             // If true, sort the edges in (x, y) order. By default it's false, most MTX files are already sorted;
		) {
	int ret_code;
	MM_typecode matcode;
	FILE *f;

	I nrows = 0;
	I ncols = 0;
	I nvals = 0;
	bool mtxinfo = false;

	if ((f = fopen(fname, "r")) == NULL) {
		std::cerr << "File " << fname << " not found" << std::endl;
		std::cerr.flush();
		exit(1);
	}

	// Read MTX banner
	if (mm_read_banner(f, &matcode) != 0) {
		printf("Could not process Matrix Market banner.\n");
		exit(1);
	}

	// Read MTX Size
	if ((ret_code = mm_read_mtx_crd_size(f, &nrows, &ncols, &nvals)) != 0)
		exit(1);
	read_tuples<I, T>(row_indices, col_indices, values, nvals, f, read_tuples_transposed, read_value_column, zero_indexed_file, debug);

	if (sort_tuples) custom_sort<I, T>(row_indices, col_indices, values);
	if (mtxinfo) mm_write_banner(stdout, matcode);
	if (mtxinfo) mm_write_mtx_crd_size(stdout, nrows, ncols, nvals);

	*num_rows = nrows;
	*num_cols = ncols;
	*num_nnz = nvals;

	return ret_code;
}