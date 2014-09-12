class Grammar
  @@grammar = {
    :start          => [[:block_body]],
    :block_body     => [[:stm_list, :ret_stm], [:stm_list], [:ret_stm]],
    :stm_list       => [[:stm, :sep, :stm_list], [:stm]],
    :ret_stm        => [[:exclamation, :expr_value]],
    :stm            => [[:msg_send], [:expr_value]],
    :msg_send       => [[:expr_value, :stm]],
    :expr_value     => [[:literal], [:par_open, :stm, :par_close]],
    :literal        => [[:empty], [:symbol], [:number], [:array], [:table], [:block], [:dollar], [:tilde], [:at]],
    :empty          => [[:brack_open, :brack_close]],
    :array          => [[:bar, :array_body], [:bar, :array_count, :bar, :array_body]],
    :array_body     => [[:array_elements], [:brack_open, :array_elements, :brack_close], [:brack_open, :brack_close]],
    :array_count    => [[:number], [:msg_send]],
    :array_elements => [[:stm, :array_elements], [:stm]],
    :table          => [[:colon, :table_body]],
    :table_body     => [[:table_entries], [:brack_open, :table_entries, :brack_close], [:brack_open, :brack_close]],
    :table_entries  => [[:table_entry, :table_entries], [:table_entry]],
    :table_entry    => [[:table_index, :arrow, :stm]],
    :table_index    => [[:symbol], [:msg_send]],
    :block          => [[:curly_open, :block_body, :curly_close], [:curly_open, :curly_close]]
  }

  def self.first(symbol)
    if !@@grammar.has_key? symbol
      symbol
    else
      @@grammar[symbol].map{ |r| Grammar.first(r[0]) }.flatten.uniq
    end
  end
end

