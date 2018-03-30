/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include "../include/pinyinime.h"
#include "../include/dicttrie.h"
#include "../include/matrixsearch.h"
#include "../include/spellingtrie.h"

#ifdef __cplusplus
extern "C" {
#endif

  using namespace ime_pinyin;

  // The maximum number of the prediction items.
  static const size_t kMaxPredictNum = 500;

  // Used to search Pinyin string and give the best candidate.
  MatrixSearch* matrix_search = NULL;

  char16 predict_buf[kMaxPredictNum][kMaxPredictSize + 1];

  bool im_open_decoder(const char *fn_sys_dict, const char *fn_usr_dict) {
    if (NULL != matrix_search)
      delete matrix_search;

    matrix_search = new MatrixSearch();
    if (NULL == matrix_search) {
      return false;
    }

    return matrix_search->init(fn_sys_dict, fn_usr_dict);
  }

  bool im_open_decoder_fd(int sys_fd, long start_offset, long length,
                          const char *fn_usr_dict) {
    if (NULL != matrix_search)
      delete matrix_search;

    matrix_search = new MatrixSearch();
    if (NULL == matrix_search)
      return false;

    return matrix_search->init_fd(sys_fd, start_offset, length, fn_usr_dict);
  }

  void im_close_decoder() {
    if (NULL != matrix_search) {
      matrix_search->close();
      delete matrix_search;
    }
    matrix_search = NULL;
  }

  void im_set_max_lens(size_t max_sps_len, size_t max_hzs_len) {
    if (NULL != matrix_search) {
      matrix_search->set_max_lens(max_sps_len, max_hzs_len);
    }
  }

  void im_flush_cache() {
    if (NULL != matrix_search)
      matrix_search->flush_cache();
  }

  // To be updated.
  size_t im_search(const char* pybuf, size_t pylen) {
    if (NULL == matrix_search)
      return 0;

    matrix_search->search(pybuf, pylen);
    return matrix_search->get_candidate_num();
  }

  size_t im_delsearch(size_t pos, bool is_pos_in_splid,
                      bool clear_fixed_this_step) {
    if (NULL == matrix_search)
      return 0;
    matrix_search->delsearch(pos, is_pos_in_splid, clear_fixed_this_step);
    return matrix_search->get_candidate_num();
  }

  void im_reset_search() {
    if (NULL == matrix_search)
      return;

    matrix_search->reset_search();
  }

  // To be removed
  size_t im_add_letter(char ch) {
    return 0;
  }

  const char* im_get_sps_str(size_t *decoded_len) {
    if (NULL == matrix_search)
      return NULL;

    return matrix_search->get_pystr(decoded_len);
  }

  char16* im_get_candidate(size_t cand_id, char16* cand_str,
                        size_t max_len) {
    if (NULL == matrix_search)
      return NULL;

    return matrix_search->get_candidate(cand_id, cand_str, max_len);
  }

  size_t im_get_spl_start_pos(const uint16 *&spl_start) {
    if (NULL == matrix_search)
      return 0;

    return matrix_search->get_spl_start(spl_start);
  }

  size_t im_choose(size_t choice_id) {
    if (NULL == matrix_search)
      return 0;

    return matrix_search->choose(choice_id);
  }

  size_t im_cancel_last_choice() {
    if (NULL == matrix_search)
      return 0;

    return matrix_search->cancel_last_choice();
  }

  size_t im_get_fixed_len() {
    if (NULL == matrix_search)
      return 0;

    return matrix_search->get_fixedlen();
  }

  // To be removed
  bool im_cancel_input() {
    return true;
  }


  size_t im_get_predicts(const char16 *his_buf,
                         char16 (*&pre_buf)[kMaxPredictSize + 1]) {
    if (NULL == his_buf)
      return 0;

    size_t fixed_len = utf16_strlen(his_buf);
    const char16 *fixed_ptr = his_buf;
    if (fixed_len > kMaxPredictSize) {
      fixed_ptr += fixed_len - kMaxPredictSize;
      fixed_len = kMaxPredictSize;
    }

    pre_buf = predict_buf;
    return matrix_search->get_predicts(his_buf, pre_buf, kMaxPredictNum);
  }

  void im_enable_shm_as_szm(bool enable) {
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    spl_trie.szm_enable_shm(enable);
  }

  void im_enable_ym_as_szm(bool enable) {
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    spl_trie.szm_enable_ym(enable);
  }

  void im_init_user_dictionary(const char *fn_usr_dict) {
    if (!matrix_search)
        return;
    matrix_search->flush_cache();
    matrix_search->init_user_dictionary(fn_usr_dict);
  }

  bool im_is_user_dictionary_enabled(void) {
    return NULL != matrix_search ? matrix_search->is_user_dictionary_enabled() : false;
  }

#ifdef __cplusplus
}
#endif
