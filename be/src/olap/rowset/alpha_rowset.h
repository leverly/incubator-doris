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

#ifndef DORIS_BE_SRC_OLAP_ROWSET_ALPHA_ROWSET_H
#define DORIS_BE_SRC_OLAP_ROWSET_ALPHA_ROWSET_H

#include "olap/rowset/rowset.h"
#include "olap/rowset/segment_group.h"
#include "olap/rowset/alpha_rowset_reader.h"
#include "olap/rowset/alpha_rowset_writer.h"
#include "olap/rowset/rowset_meta.h"
#include "olap/data_dir.h"
#include "olap/tuple.h"

#include <vector>
#include <memory>

namespace doris {

class AlphaRowset : public Rowset {
public:
    AlphaRowset(const TabletSchema* schema, const std::string rowset_path,
                DataDir* data_dir, RowsetMetaSharedPtr rowset_meta);
    virtual ~AlphaRowset() {}

    OLAPStatus init() override;

    std::shared_ptr<RowsetReader> create_reader() override;

    OLAPStatus copy(RowsetWriter* dest_rowset_writer) override;

    OLAPStatus remove() override;

    void to_rowset_pb(RowsetMetaPB* rs_meta) override;

    RowsetMetaSharedPtr rowset_meta() const override;

    int data_disk_size() const override;

    int index_disk_size() const override;

    bool empty() const override;

    bool zero_num_rows() const override;

    size_t num_rows() const override;

    Version version() const override;

    void set_version(Version version) override;

    int64_t end_version() const override;

    int64_t start_version() const override;

    VersionHash version_hash() const override;

    void set_version_hash(VersionHash version_hash) override;

    bool in_use() const override;

    void acquire() override;

    void release() override;
    
    int64_t ref_count() const override;

    virtual OLAPStatus make_snapshot(const std::string& snapshot_path,
                                     std::vector<std::string>* success_files);

    OLAPStatus remove_old_files(std::vector<std::string>* files_to_remove) override;

    RowsetId rowset_id() const override;

    int64_t creation_time() override;

    bool is_pending() const override;

    int64_t txn_id() const override;

    bool delete_flag() override;

    OLAPStatus split_range(
            const RowCursor& start_key,
            const RowCursor& end_key,
            uint64_t request_block_row_count,
            vector<OlapTuple>* ranges);

private:
    OLAPStatus _init_segment_groups();

    OLAPStatus _init_pending_segment_groups();

    OLAPStatus _init_non_pending_segment_groups();

    std::shared_ptr<SegmentGroup> _segment_group_with_largest_size();

private:
    friend class AlphaRowsetWriter;
    const TabletSchema* _schema;
    std::string _rowset_path;
    DataDir* _data_dir;
    RowsetMetaSharedPtr _rowset_meta;
    std::vector<std::shared_ptr<SegmentGroup>> _segment_groups;
    int _segment_group_size;
    bool _is_cumulative_rowset;
    bool _is_pending_rowset;
    atomic_t _ref_count;
};

} // namespace doris

#endif // DORIS_BE_SRC_OLAP_ROWSET_ALPHA_ROWSET_H