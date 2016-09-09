#include <Rcpp.h>


// Slice a window of rows out of a matrix.
Rcpp::NumericMatrix mat_to_win( Rcpp::NumericMatrix myMat, 
                                int start_row, 
                                int end_row
){
  Rcpp::NumericMatrix retMatrix( end_row - start_row + 1, myMat.ncol() );
  
  int i = 0;
  int j = 0;
  
  for(i=0; i<myMat.ncol(); i++){
    for(j=start_row; j<=end_row; j++){
      retMatrix(j - start_row + 0, i) = myMat(j,i);
    }
  }
  
  return(retMatrix);
}


// Count non missing values in a NumericMatrix.
Rcpp::NumericVector count_nonNA( Rcpp::NumericMatrix myMat ){
  Rcpp::NumericVector myCounts( myMat.ncol() );

  int i = 0;
  int j = 0;
  
  // Initialize to zero.
  for(i=0; i<myCounts.size(); i++){
    myCounts(i) = 0;
  }

  for(i=0; i<myMat.ncol(); i++){
    for(j=0; j<myMat.nrow() - 1; j++){
      if( !Rcpp::NumericVector::is_na( myMat(j,i) ) ){
        myCounts(i) = myCounts(i) + 1;
      }
    }
  }

  return(myCounts);  
}

// Sort data into bins based on breaks (boundaries).
std::vector<int> bin_data( Rcpp::NumericVector myFreqs,
                           std::vector<double> breaks
//                           Rcpp::NumericVector breaks
){
  std::vector<int> counts ( breaks.size() - 1, 0 );
  
  int i = 0;
  int j = 0;
  
  for(i=0; i<myFreqs.size(); i++){
//    Rcpp::Rcout << "myFreqs(i): " << myFreqs(i) << "\n";
//    if( myFreqs(i) == 1 ){
//      Rcpp::Rcout << "myFreqs(i) is one!\n";
//    }
      
    for(j=0; j < counts.size() - 1; j++){
      if( myFreqs(i) >= breaks[j] & myFreqs(i) < breaks[j+1] ){
        counts[j]++;
//        Rcpp::Rcout << j << ": breaks[j]: " << breaks[j] << ", breaks[j+1]: " << breaks[j+1] << "\n";
      }
    }
//    Rcpp::Rcout << "j: " << j << " breaks[j]: " << breaks[j] << ", breaks[j+1]: " << breaks[j+1] << "\n";
//    if( 1 <= breaks[j+1] + 0.0000001 ){
//      Rcpp::Rcout << " 1 <= breaks(j+1) is TRUE!\n";
//    }
    if( myFreqs(i) >= breaks[j] & myFreqs(i) <= breaks[j+1] + 0.0000001 ){
      counts[j]++;
//      Rcpp::Rcout << j << ": breaks[j]: " << breaks[j] << ", breaks[j+1]: " << breaks[j+1] << "\n";
    }
//    Rcpp::Rcout << "\n";
  }
  
  return(counts);
}

//Rcpp::NumericVector find_one_peak( Rcpp::NumericVector myFreqs ){
double find_one_peak( Rcpp::NumericVector myFreqs,
//                      Rcpp::NumericVector breaks,
                      std::vector<double> breaks,
                      Rcpp::NumericVector mids
                      ){
  double myPeak = 0;
  int i = 0;

  // Bin the data.
  std::vector<int> counts ( mids.size(), 0 );
  counts = bin_data(myFreqs, breaks);
  
//  Rcpp::Rcout << "Counts\tMids\n";
//  Rcpp::Rcout << "0: " << counts[0] << "\t" << mids(0) << "\n";
//  for(i=1;i<counts.size(); i++){
//    Rcpp::Rcout << i << ": " << counts[i] << "\t" << mids(i) << "\n";
//  }
//  Rcpp::Rcout << "\n";
  
  // Find the peak.
  int max_peak = 0;
  for(i=1; i<counts.size(); i++){
    if( counts[i] > counts[max_peak] ){
      max_peak = i;
    }
  }
  
  myPeak = mids[max_peak];
  return( myPeak );
}


// Find peaks from frequency values [0-1] from a ingle window of data.
//
Rcpp::NumericVector find_peaks( Rcpp::NumericMatrix myMat, float bin_width ){

  int i = 0;
  int j = 0;
  int k = 0;
  
  // Create return vector and initialize to zero.
  Rcpp::NumericVector myPeaks( myMat.ncol() );
  for(i=0; i<myPeaks.size(); i++){
    myPeaks(i) = 0;
  }

  int nbins = 1 / bin_width;
//  Rcpp::NumericVector breaks( nbins + 1 );
  std::vector<double> breaks ( nbins + 1, 0 );
  Rcpp::NumericVector mids( nbins );
//  Rcpp::NumericVector counts( nbins );
  
  // Test thet 1/bin_width does not have a remainder.
  if( 1/bin_width - nbins > 0 ){
    Rcpp::Rcerr << "1/bin_width has a remainder.\nThis will result in uneven bins.\nPlease try another bin_width.\n";
    return(myPeaks);
  }
  
  // Initialize vectors.
  breaks[0] = 0;
  for(i=0; i<mids.size(); i++ ){
    breaks[i+1] = breaks[i] + bin_width;
    mids(i) = breaks[i] + bin_width/2;
//    counts(i) = 0;
  }
//  breaks( breaks.size() - 1 ) = 1;
  
  /*
  Rcpp::Rcout << "Breaks (n=" << breaks.size() << "): " << breaks(0);
  for(i=1; i<10; i++){
      Rcpp::Rcout << ", " << breaks(i);
  }
  Rcpp::Rcout << ", ... " << breaks( breaks.size() - 1 ) << "\n";
  
  Rcpp::Rcout << "Mids (n=" << mids.size() << "): " << mids(0);
  for(i=1; i<10; i++){
      Rcpp::Rcout << ", " << mids(i);
  }
  Rcpp::Rcout << ", ... " << mids( mids.size() - 1 ) << "\n";
  */
  
  for(i=0; i<myMat.ncol(); i++){ // Column (sample) counter.
    myPeaks(i) = find_one_peak( myMat( Rcpp::_, i), breaks, mids );
  }
  
  // Bin the data.
//  for(i=0; i<myMat.ncol(); i++){ // Column (sample) counter.
//    for(j=0; j<myMat.nrow(); j++){ // Row (variant) counter.
//      if( myMat(j,i) != NA_REAL ){
        //Rcpp::Rcout << "\n  counts.size(): " << counts.size() << "\n";
//        Rcpp::Rcout << "Freq: " << myMat(j,i) << "\n";

//        for(k=0; k<counts.size()-1; k++){ // Bin counter.
//          if( myMat(j,i) >= breaks(k) & myMat(j,i) < breaks(k + 1) ){
//            counts(k)++;
//            Rcpp::Rcout << "    Binned: " << "\t" << breaks(k)  << "\t" << breaks(k + 1) << "\n";
//          }
//        }
        // Last bin.
//        Rcpp::Rcout << "Last break: " << k << ": " << breaks(k + 1) << "\n";
//        if( myMat(j,i) == 1 ){
//          Rcpp::Rcout << "Freqing 1, k: " << k << ", breaks(k):" << breaks(k) << ", breaks(k+1): " << breaks(k+1) << "\n";
//          if( myMat(j,i) == breaks(k+1) ){
//            Rcpp::Rcout << "  This should bin!\n";
//          }
//          if( breaks(k+1) == myMat(j,i) ){
//            Rcpp::Rcout << "  breaks(k+1) == 1!\n";
//          }
//        }

//        if( myMat(j,i) >= breaks(k) & myMat(j,i) <= breaks(k + 1) ){
//          counts(k)++;
//          Rcpp::Rcout << "    Binned: " << "\t" << breaks(k) << "\t" << breaks(k + 1) << "\n";
//        }
//      }
//    }
    
    // Report counts.
    
    
//    Rcpp::Rcout << "\nSample: " << i << "\n";
//    Rcpp::Rcout << "Counts\tmids\n";
//    for(k=0; k<counts.size(); k++){
//      Rcpp::Rcout << counts[k] << "\t" << mids[k] << "\n";
//    }
//    Rcpp::Rcout << "\n";
    

    
    // Data should be binned.
    // Now find the bin with the greatest number of counts.
//    int max_peak = 0;
//    Rcpp::Rcout << "Counts: " << counts(0) << " mid: " << mids(0);
//    for(k=1; k<counts.size(); k++){ // Bin counter.
//      Rcpp::Rcout << ", " << counts(k) << " mid: " << mids(k);
//      if( counts[k] > counts[max_peak] ){
//        max_peak = k;
//        Rcpp::Rcout << "\nnew max at: " << counts(k)<< " mid: " << mids(k) << "\n";
//      }
//    }
//    Rcpp::Rcout << ", done!\n\n";
//    myPeaks[i] = mids[max_peak];
//  }

  return(myPeaks);
}


//' 
//' @rdname freq_peak
//' 
//' @title freq_peak
//' @description Find peaks in frequency data.
//' 
//' @param myMat a matrix of frequencies [0-1].
//' @param pos a numeric vector describing the position of variants in myMat.
//' @param winsize sliding window size.
//' @param bin_width Width of bins to summarize ferequencies in (0-1].
//' @param count logical specifying to count the number of non-NA values intead of reporting peak.
//' 
//' @details
//' More to come.
//' 
//' @return 
//' A list
//' 
//' @examples
//' data(vcfR_example)
//' gt <- extract.gt(vcf)
//' hets <- is_het(gt)
//' # Censor non-heterozygous positions.
//' is.na(vcf@gt[,-1][!hets]) <- TRUE
//' # Extract allele depths.
//' ad <- extract.gt(vcf, element = "AD")
//' ad1 <- masplit(ad, record = 1)
//' ad2 <- masplit(ad, record = 2)
//' freq1 <- ad1/(ad1+ad2)
//' freq2 <- ad2/(ad1+ad2)
//' myPeaks1 <- freq_peak(freq1, getPOS(vcf))
//' myCounts1 <- freq_peak(freq1, getPOS(vcf), count = TRUE)
//' is.na(myPeaks1$peaks[myCounts1$peaks < 20]) <- TRUE
//' myPeaks2 <- freq_peak(freq2, getPOS(vcf))
//' myCounts2 <- freq_peak(freq2, getPOS(vcf), count = TRUE)
//' is.na(myPeaks2$peaks[myCounts2$peaks < 20]) <- TRUE
//' #myPeaks <- freq_peak(freqs[1:115,], getPOS(vcf)[1:115])
//' 
//' # Visualize
//' mySample <- "P17777us22"
//' myWin <- 2
//' hist(freq1[myPeaks1$wins[myWin,'START_row']:myPeaks1$wins[myWin,'END_row'], mySample], 
//'      breaks=seq(0,1,by=0.02), col="#A6CEE3", main="", xlab="", xaxt="n")
//' hist(freq2[myPeaks2$wins[myWin,'START_row']:myPeaks2$wins[myWin,'END_row'], mySample], 
//'      breaks=seq(0,1,by=0.02), col="#1F78B4", main="", xlab="", xaxt="n", add = TRUE)
//' axis(side=1, at=c(0,0.25,0.333,0.5,0.666,0.75,1), 
//'      labels=c(0,'1/4','1/3','1/2','2/3','3/4',1), las=3)
//' abline(v=myPeaks1$peaks[myWin,mySample], col=2, lwd=2)
//' abline(v=myPeaks2$peaks[myWin,mySample], col=2, lwd=2)
//' 
//' # Visualize #2
//' plot(getPOS(vcf), freq1[,mySample], ylim=c(0,1), type="n", yaxt='n', 
//'      main = mySample, xlab = "POS", ylab = "Allele balance")
//' axis(side=2, at=c(0,0.25,0.333,0.5,0.666,0.75,1), 
//'      labels=c(0,'1/4','1/3','1/2','2/3','3/4',1), las=1)
//' abline(h=c(0.25,0.333,0.5,0.666,0.75), col=8)
//' points(getPOS(vcf), freq1[,mySample], pch = 20, col= "#A6CEE3")
//' points(getPOS(vcf), freq2[,mySample], pch = 20, col= "#1F78B4")
//' segments(x0=myPeaks1$wins[,'START_pos'], y0=myPeaks1$peaks[,mySample],
//'          x1=myPeaks1$wins[,'END_pos'], lwd=3)
//' segments(x0=myPeaks1$wins[,'START_pos'], y0=myPeaks2$peaks[,mySample],
//'          x1=myPeaks1$wins[,'END_pos'], lwd=3)
//' 
//' 
//' 
//' @export
// [[Rcpp::export]]
Rcpp::List freq_peak(Rcpp::NumericMatrix myMat,
                     Rcpp::NumericVector pos,
                     int winsize = 10000,
                     float bin_width = 0.02,
                     Rcpp::LogicalVector count = false
                     ){
  int i = 0;
  int j = 0;
  
  // NA matrix to return in case of unexpected results.
  Rcpp::NumericMatrix naMat( 1, 1 );
  naMat(0,0) = NA_REAL;
  
  // Create a matrix of windows.
// Rcpp::Rcout << "pos.size() is: " << pos.size() << ".\n"; 
  int max_pos = pos[ pos.size() - 1 ] / winsize + 1;
// Rcpp::Rcout << "max_pos is: " << max_pos << ".\n"; 
  Rcpp::NumericMatrix wins( max_pos, 6);
  Rcpp::StringVector rownames( max_pos );
  for(i=0; i<max_pos; i++){
    wins(i,0) = i * winsize + 1;
    wins(i,1) = i * winsize + winsize;
    rownames(i) = "win" + std::to_string(i+1);
  }
//  Rcpp::Rcout << "wins initialized!\n";
  Rcpp::StringVector colnames(6);
  colnames(0) = "START";
  colnames(1) = "END";
  colnames(2) = "START_row";
  colnames(3) = "END_row";
  colnames(4) = "START_pos";
  colnames(5) = "END_pos";
  wins.attr("dimnames") = Rcpp::List::create(rownames, colnames);

  // Initialize a freq matrix.
  Rcpp::NumericMatrix freqs( max_pos, myMat.ncol() );
//  Rcpp::Rcout << "Trying dimnames.\n";
  Rcpp::StringVector myColNames = Rcpp::colnames(myMat);
//  Rcpp::Rcout << "myColNames.size(): " << myColNames.size() << "\n";

  Rcpp::rownames(freqs) = rownames;
  if( myColNames.size() > 0 ){
    Rcpp::colnames(freqs) = myColNames;
  }
  
//  Rcpp::Rcout << "Finished dimnames.\n";
  
  // Find windows in pos.
  int win_num = 0;
  i = 0;

  // First row.
//  Rcpp::Rcout << "First row.\n";
  while( pos(i) < wins(win_num,0) ){
    win_num++;
  }
  wins(win_num,2) = i + 1;
  wins(win_num,4) = pos(0);
  
  // Remaining rows.
//  Rcpp::Rcout << "Windowing.\n";
  for(i=1; i<myMat.nrow(); i++){
    
    if( pos(i) > wins(win_num,1) ){
      // Increment window.
//      Rcpp::Rcout << "  New window, pos(i): " << pos(i) << " wins(win_num,0): " << wins(win_num,0) << " wins(win_num,1): " << wins(win_num,1) << "\n";
      wins(win_num,3) = i;
      wins(win_num,5) = pos(i-1);
      
      while( pos(i) > wins(win_num,1) ){
//        win_num++;
        win_num = win_num + 1;
//        Rcpp::Rcout << "    Incrementing win_num: " << win_num << "\n";
      }
//      Rcpp::Rcout << "    win_num: " << win_num << "\n";
      wins(win_num,2) = i + 1;
      wins(win_num,4) = pos(i);
    }
  }
  
  // Last row.
  wins(win_num,3) = i;
  wins(win_num,5) = pos(i-1);

  // Windowize and process.

  // Check bin_width validity.
  if( !count(0) ){
    // Positive bin width.
    if( bin_width <= 0 ){
      Rcpp::Rcerr << "bin_width must be greater than zero, please try another bin_width.\n";
      Rcpp::List myList = Rcpp::List::create(
        Rcpp::Named("wins") = wins,
        Rcpp::Named("peaks") = naMat
      );
      return( myList );
    }
    
    // bin width less than one.
    if( bin_width > 1 ){
      Rcpp::Rcerr << "bin_width must be no greater than one, please try another bin_width.\n";
      Rcpp::List myList = Rcpp::List::create(
        Rcpp::Named("wins") = wins,
        Rcpp::Named("peaks") = naMat
      );
      return( myList );
    }
    
    // No remainder to bin width.
    int nbins = 1 / bin_width;
    if( 1/bin_width - nbins > 0 ){
      Rcpp::Rcerr << "1/bin_width has a remainder, please try another bin_width.\n";
      Rcpp::List myList = Rcpp::List::create(
        Rcpp::Named("wins") = wins,
        Rcpp::Named("peaks") = naMat
      );
    return( myList );
    }
  }
  
  // Window counter.
  for(i=0; i<freqs.nrow(); i++){
    Rcpp::NumericMatrix myWin(wins(i,3) - wins(i,2) + 1, freqs.ncol());
    // Remember, R=1-based, C++=0-based!
    myWin = mat_to_win(myMat, wins(i,2) - 1, wins(i,3) - 1 );

    /*
    Rcpp::Rcout << "myWin nrow: " << myWin.nrow() << "\n";
    Rcpp::Rcout << "myWin ncol: " << myWin.ncol() << "\n";
    
    Rcpp::Rcout << "Freqs: " << myWin(0,1);
    for(j=1; j<myWin.nrow(); j++){
      Rcpp::Rcout << ", " << myWin(j,0);
    }
    Rcpp::Rcout << "\n";
    */
    
//    Rcpp::Rcout << "count(0):" << count(0) << "\n";
    if( count(0) ){
//          Rcpp::Rcout << "count(0):" << count(0) << " must be true!\n";
      freqs(i,Rcpp::_) = count_nonNA( myWin );
    } else {
      freqs(i,Rcpp::_) = find_peaks( myWin, bin_width );
    }
  }
  
  // Create the return List.
  Rcpp::List myList = Rcpp::List::create(
    Rcpp::Named("wins") = wins,
    Rcpp::Named("peaks") = freqs
  );
  
  return(myList);
}



