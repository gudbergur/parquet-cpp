// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <iostream>
#include <list>
#include <memory>

#include "parquet/column/reader.h"
#include "parquet/column/writer.h"
#include "parquet/file/reader.h"
#include "parquet/file/writer.h"
#include "parquet/types.h"
#include "parquet/util/input.h"
#include "parquet/util/output.h"
//#include "parquet/column/test-specialization.h"
//#include "parquet/column/test-util.h"

namespace parquet {

namespace blah {

using schema::GroupNode;
using schema::NodePtr;
using schema::PrimitiveNode;

int main(int argc, char** argv) {
    if (argc > 5 || argc < 2) {
      std::cerr << "Usage: parquet_writer <file>"
      << std::endl;
    return -1;
    }


    auto num_columns = 1;

    std::shared_ptr<LocalFileOutputStream> sink(new LocalFileOutputStream("hello.parquet"));

    schema::NodePtr node_;
    SchemaDescriptor schema_;
    std::vector<schema::NodePtr> fields;

    std::string name = "TESTCOLUMNNN";
    fields.push_back(schema::PrimitiveNode::Make(
        name, Repetition::REQUIRED, Int32Type::type_num, LogicalType::NONE, 12)); //FLBA_LENGTH
    node_ = schema::GroupNode::Make("schema", Repetition::REQUIRED, fields);
    schema_.Init(node_);

    auto gnode = std::static_pointer_cast<GroupNode>(node_);

    WriterProperties::Builder prop_builder;

    for (int i = 0; i < num_columns; ++i) {
        prop_builder.compression(schema_.Column(i)->name(), Compression::UNCOMPRESSED);
    }

    std::shared_ptr<WriterProperties> writer_properties = prop_builder.build();

    auto file_writer = ParquetFileWriter::Open(sink, gnode, writer_properties);
    auto num_values = 100;
    auto row_group_writer = file_writer->AppendRowGroup(100);

    std::vector<Int32Type> values_;
    std::vector<int16_t> def_levels_;
    std::vector<uint8_t> buffer_;

    def_levels_.resize(num_values);
    values_.resize(num_values);

//    InitValues<Int32Type>(num_values, values_, buffer_);
    Int32Type *values_ptr_ = values_.data();

    std::fill(def_levels_.begin(), def_levels_.end(), 1);

//    for (int i = 0; i < num_columns; ++i) {
      auto column_writer = static_cast<TypedColumnWriter<Int32Type> *>(row_group_writer->NextColumn());
      column_writer->WriteBatch(100, def_levels_.data(), nullptr, values_ptr_);
      column_writer->Close();
//    }

    row_group_writer->Close();
    file_writer->Close();

    return 0;

}

}

}
