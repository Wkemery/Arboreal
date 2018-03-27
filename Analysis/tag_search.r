startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataDone/tag_search_time.txt", header = TRUE)


plot(time~files, data = startup)
plot(time~tags, data = startup)
plot(log(time)~numtags, data = startup)


fit<-lm(log(time)~ numtags + I(numtags^2) + I(numtags^3)+ log(numtags)+ foundfiles, data = startup)
summary(fit)
plot(fit)


plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$foundfiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")
